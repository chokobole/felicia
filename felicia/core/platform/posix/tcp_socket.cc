// Copyright 2014 The Chromium Authors. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// =====================================================================
// Modifications copyright (C) 2019 felicia

#include "felicia/core/platform/tcp_socket.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "felicia/core/lib/message_loop/message_loop_current.h"
#include "felicia/core/lib/net/ip_endpoint.h"
#include "felicia/core/lib/net/sockaddr_storage.h"
#include "felicia/core/platform/file_util.h"
#include "felicia/core/platform/posix/eintr_wrapper.h"

namespace felicia {
namespace net {

namespace {

int MapAcceptError(int os_error) {
  switch (os_error) {
    // If the client aborts the connection before the server calls accept,
    // POSIX specifies accept should fail with ECONNABORTED. The server can
    // ignore the error and just call accept again, so we map the error to
    // ERR_IO_PENDING. See UNIX Network Programming, Vol. 1, 3rd Ed., Sec.
    // 5.11, "Connection Abort before accept Returns".
    case ECONNABORTED:
      return ERR_IO_PENDING;
    default:
      return MapSystemError(os_error);
  }
}

int MapConnectError(int os_error) {
  switch (os_error) {
    case EINPROGRESS:
      return ERR_IO_PENDING;
    case EACCES:
      return ERR_NETWORK_ACCESS_DENIED;
    case ETIMEDOUT:
      return ERR_CONNECTION_TIMED_OUT;
    default: {
      int net_error = MapSystemError(os_error);
      if (net_error == ERR_FAILED)
        return ERR_CONNECTION_FAILED;  // More specific than ERR_FAILED.
      return net_error;
    }
  }
}

}  // namespace

TCPSocket::TCPSocket()
    : accept_socket_watcher_(FROM_HERE),
      read_socket_watcher_(FROM_HERE),
      read_buf_len_(0),
      write_socket_watcher_(FROM_HERE),
      write_buf_len_(0),
      waiting_connect_(false) {}

TCPSocket::~TCPSocket() { (void)Close(); }

int TCPSocket::Open(AddressFamily family) {
  int address_family = ConvertAddressFamily(family);
  fd_ = CreatePlatformSocket(address_family, SOCK_STREAM,
                             address_family == AF_UNIX ? 0 : IPPROTO_TCP);
  if (fd_ < 0) {
    PLOG(ERROR) << "CreatePlatformSocket() failed";
    return MapSystemError(errno);
  }

  if (!SetNonBlocking(fd_)) {
    int rv = MapSystemError(errno);
    Close();
    return rv;
  }

  return OK;
}

int TCPSocket::AdoptConnectedSocket(SocketDescriptor socket,
                                    const IPEndPoint& peer_address) {
  int rv = AdoptUnconnectedSocket(socket);
  if (rv != OK) return rv;

  SetPeerAddress(peer_address);
  return OK;
}

int TCPSocket::AdoptUnconnectedSocket(SocketDescriptor socket) {
  DCHECK(!IsValid());

  fd_ = socket;

  if (!SetNonBlocking(fd_)) {
    int rv = MapSystemError(errno);
    Close();
    return rv;
  }

  return OK;
}

SocketDescriptor TCPSocket::ReleaseConnectedSocket() {
  StopWatchingAndCleanUp();
  SocketDescriptor socket_fd = fd_;
  fd_ = kInvalidSocket;
  return socket_fd;
}

int TCPSocket::Close() {
  DCHECK(IsValid());

  int rv = close(fd_);
  if (rv < 0) {
    if (errno == EINPROGRESS || errno == EINTR) {
      // Socket is closing
      fd_ = kInvalidSocket;
      return OK;
    }

    return MapSystemError(errno);
  }

  fd_ = kInvalidSocket;
  return 0;
}

int TCPSocket::Listen(int backlog) {
  DCHECK(IsValid());
  DCHECK_LT(0, backlog);

  int rv = listen(fd_, backlog);
  if (rv < 0) {
    PLOG(ERROR) << "listen() failed";
    return MapSystemError(errno);
  }

  return OK;
}

int TCPSocket::Accept(std::unique_ptr<TCPSocket>* socket,
                      CompletionOnceCallback callback) {
  DCHECK(IsValid());
  DCHECK(!accept_callback_);
  DCHECK(socket);
  DCHECK(callback);

  int rv = DoAccept(socket);
  if (rv != ERR_IO_PENDING) return rv;

  if (!MessageLoopCurrent::Get()->WatchFileDescriptor(
          fd_, true, MessagePump::WATCH_READ, &accept_socket_watcher_, this)) {
    PLOG(ERROR) << "WatchFileDescriptor failed on accept";
    return MapSystemError(errno);
  }

  accept_socket_ = socket;
  accept_callback_ = std::move(callback);
  return ERR_IO_PENDING;
}

int TCPSocket::Connect(const IPEndPoint& endpoint,
                       CompletionOnceCallback callback) {
  DCHECK(IsValid());
  DCHECK(callback);
  DCHECK(!waiting_connect_);

  SetPeerAddress(endpoint);

  int rv = DoConnect();
  if (rv != ERR_IO_PENDING) return rv;
  if (!MessageLoopCurrent::Get()->WatchFileDescriptor(
          fd_, true, MessagePump::WATCH_WRITE, &write_socket_watcher_, this)) {
    PLOG(ERROR) << "WatchFileDescriptor failed on connect";
    return MapSystemError(errno);
  }

  // There is a race-condition in the above code if the kernel receive a RST
  // packet for the "connect" call before the registration of the socket file
  // descriptor to the message loop pump. On most platform it is benign as the
  // message loop pump is awakened for that socket in an error state, but on
  // iOS this does not happens. Check the status of the socket at this point
  // and if in error, consider the connection as failed.
  int os_error = 0;
  socklen_t len = sizeof(os_error);
  if (getsockopt(fd_, SOL_SOCKET, SO_ERROR, &os_error, &len) == 0) {
    // TCPSocket expects errno to be set.
    errno = os_error;
  }

  rv = MapConnectError(errno);
  if (rv != OK && rv != ERR_IO_PENDING) {
    write_socket_watcher_.StopWatchingFileDescriptor();
    return rv;
  }

  write_callback_ = callback;
  waiting_connect_ = true;
  return ERR_IO_PENDING;
}

bool TCPSocket::IsConnected() const {
  if (!IsValid() || waiting_connect_) return false;

  // Checks if connection is alive.
  char c;
  int rv = HANDLE_EINTR(recv(fd_, &c, 1, MSG_PEEK));
  if (rv == 0) return false;
  if (rv == -1 && errno != EAGAIN && errno != EWOULDBLOCK) return false;

  return true;
}

bool TCPSocket::IsConnectedAndIdle() const {
  if (!IsValid() || waiting_connect_) return false;

  // Check if connection is alive and we haven't received any data
  // unexpectedly.
  char c;
  int rv = HANDLE_EINTR(recv(fd_, &c, 1, MSG_PEEK));
  if (rv >= 0) return false;
  if (errno != EAGAIN && errno != EWOULDBLOCK) return false;

  return true;
}

size_t TCPSocket::Read(Buffer* buf, size_t buf_len,
                       CompletionOnceCallback callback) {
  // Use base::Unretained() is safe here because OnFileCanReadWithoutBlocking()
  // won't be called if |this| is gone.
  int rv = ReadIfReady(
      buf, buf_len,
      std::bind(&TCPSocket::RetryRead, this, std::placeholders::_1));
  if (rv == ERR_IO_PENDING) {
    read_buf_.reset(buf);
    read_buf_len_ = buf_len;
    read_callback_ = std::move(callback);
  }
  return rv;
}

int TCPSocket::ReadIfReady(Buffer* buf, int buf_len,
                           CompletionOnceCallback callback) {
  DCHECK(IsValid());
  DCHECK(!waiting_connect_);
  CHECK(!read_if_ready_callback_);
  DCHECK(callback);
  DCHECK_LT(0, buf_len);

  int rv = DoRead(buf, buf_len);
  if (rv != ERR_IO_PENDING) return rv;

  if (!MessageLoopCurrent::Get()->WatchFileDescriptor(
          fd_, true, MessagePump::WATCH_READ, &read_socket_watcher_, this)) {
    PLOG(ERROR) << "WatchFileDescriptor failed on read";
    return MapSystemError(errno);
  }

  read_if_ready_callback_ = std::move(callback);
  return ERR_IO_PENDING;
}

int TCPSocket::CancelReadIfReady() {
  DCHECK(read_if_ready_callback_);

  bool ok = read_socket_watcher_.StopWatchingFileDescriptor();
  DCHECK(ok);

  read_if_ready_callback_ = nullptr;
  return net::OK;
}

size_t TCPSocket::Write(Buffer* buf, size_t buf_len,
                        CompletionOnceCallback callback) {
  DCHECK(IsValid());
  DCHECK(!waiting_connect_);
  CHECK(!write_callback_);
  // Synchronous operation not supported
  DCHECK(callback);
  DCHECK_LT(0, buf_len);

  int rv = DoWrite(buf, buf_len);
  if (rv == ERR_IO_PENDING)
    rv = WaitForWrite(buf, buf_len, std::move(callback));
  return rv;
}

int TCPSocket::WaitForWrite(Buffer* buf, int buf_len,
                            CompletionOnceCallback callback) {
  DCHECK(IsValid());
  DCHECK(!write_callback_);
  // Synchronous operation not supported
  DCHECK(callback);
  DCHECK_LT(0, buf_len);

  if (!MessageLoopCurrent::Get()->WatchFileDescriptor(
          fd_, true, MessagePump::WATCH_WRITE, &write_socket_watcher_, this)) {
    PLOG(ERROR) << "WatchFileDescriptor failed on write";
    return MapSystemError(errno);
  }

  write_buf_.reset(buf);
  write_buf_len_ = buf_len;
  write_callback_ = std::move(callback);
  return ERR_IO_PENDING;
}

void TCPSocket::OnFileCanReadWithoutBlocking(int fd) {
  if (accept_callback_) {
    AcceptCompleted();
  } else {
    DCHECK(read_if_ready_callback_);
    ReadCompleted();
  }
}

void TCPSocket::OnFileCanWriteWithoutBlocking(int fd) {
  DCHECK(write_callback_);
  if (waiting_connect_) {
    ConnectCompleted();
  } else {
    WriteCompleted();
  }
}

int TCPSocket::DoAccept(std::unique_ptr<TCPSocket>* socket) {
  SockaddrStorage new_peer_address;
  int new_socket = HANDLE_EINTR(
      accept(fd_, new_peer_address.addr, &new_peer_address.addr_len));
  if (new_socket < 0) return MapAcceptError(errno);

  IPEndPoint new_endpoint;
  (void)new_endpoint.FromSockAddr(new_peer_address.addr,
                                  new_peer_address.addr_len);

  std::unique_ptr<TCPSocket> accepted_socket(new TCPSocket);
  int rv = accepted_socket->AdoptConnectedSocket(new_socket, new_endpoint);
  if (rv != OK) return rv;

  *socket = std::move(accepted_socket);
  return OK;
}

void TCPSocket::AcceptCompleted() {
  DCHECK(accept_socket_);
  int rv = DoAccept(accept_socket_);
  if (rv == ERR_IO_PENDING) return;

  bool ok = accept_socket_watcher_.StopWatchingFileDescriptor();
  DCHECK(ok);
  accept_socket_ = nullptr;
  CONSUME_COMPLETE_ONCE_CALLBACK(accept_callback_, rv);
}

int TCPSocket::DoConnect() {
  DCHECK(IsValid());

  SockaddrStorage address;
  if (!peer_address_->ToSockAddr(address.addr, &address.addr_len))
    return ERR_ADDRESS_INVALID;

  int rv = HANDLE_EINTR(connect(fd_, address.addr, address.addr_len));
  DCHECK_GE(0, rv);
  return rv == 0 ? OK : MapConnectError(errno);
}

void TCPSocket::ConnectCompleted() {
  // Get the error that connect() completed with.
  int os_error = 0;
  socklen_t len = sizeof(os_error);
  if (getsockopt(fd_, SOL_SOCKET, SO_ERROR, &os_error, &len) == 0) {
    // TCPSocket expects errno to be set.
    errno = os_error;
  }

  int rv = MapConnectError(errno);
  if (rv == ERR_IO_PENDING) return;

  bool ok = write_socket_watcher_.StopWatchingFileDescriptor();
  DCHECK(ok);
  waiting_connect_ = false;
  CONSUME_COMPLETE_ONCE_CALLBACK(write_callback_, rv);
}

int TCPSocket::DoRead(Buffer* buf, int buf_len) {
  int rv = HANDLE_EINTR(read(fd_, buf->data(), buf_len));
  return rv >= 0 ? rv : MapSystemError(errno);
}

void TCPSocket::RetryRead(int rv) {
  DCHECK(read_callback_);
  DCHECK(read_buf_);
  DCHECK_LT(0, read_buf_len_);

  if (rv == OK) {
    rv = ReadIfReady(
        read_buf_.get(), read_buf_len_,
        std::bind(&TCPSocket::RetryRead, this, std::placeholders::_1));
    if (rv == ERR_IO_PENDING) return;
  }
  read_buf_ = nullptr;
  read_buf_len_ = 0;
  CONSUME_COMPLETE_ONCE_CALLBACK(read_callback_, rv);
}

void TCPSocket::ReadCompleted() {
  DCHECK(read_if_ready_callback_);

  bool ok = read_socket_watcher_.StopWatchingFileDescriptor();
  DCHECK(ok);
  CONSUME_COMPLETE_ONCE_CALLBACK(read_if_ready_callback_, OK);
}

int TCPSocket::DoWrite(Buffer* buf, int buf_len) {
#if defined(PLATFORM_LINUX) || defined(PLATFORM_ANDROID)
  // Disable SIGPIPE for this write. Although Chromium globally disables
  // SIGPIPE, the net stack may be used in other consumers which do not do
  // this. MSG_NOSIGNAL is a Linux-only API. On OS X, this is a setsockopt on
  // socket creation.
  int rv = HANDLE_EINTR(send(fd_, buf->data(), buf_len, MSG_NOSIGNAL));
#else
  int rv = HANDLE_EINTR(write(fd_, buf->data(), buf_len));
#endif
  return rv >= 0 ? rv : MapSystemError(errno);
}

void TCPSocket::WriteCompleted() {
  int rv = DoWrite(write_buf_.get(), write_buf_len_);
  if (rv == ERR_IO_PENDING) return;

  bool ok = write_socket_watcher_.StopWatchingFileDescriptor();
  DCHECK(ok);
  write_buf_ = nullptr;
  write_buf_len_ = 0;
  CONSUME_COMPLETE_ONCE_CALLBACK(write_callback_, rv);
}

void TCPSocket::StopWatchingAndCleanUp() {
  bool ok = accept_socket_watcher_.StopWatchingFileDescriptor();
  DCHECK(ok);
  ok = read_socket_watcher_.StopWatchingFileDescriptor();
  DCHECK(ok);
  ok = write_socket_watcher_.StopWatchingFileDescriptor();
  DCHECK(ok);

  if (accept_callback_) {
    accept_socket_ = NULL;
    accept_callback_ = nullptr;
  }

  if (read_callback_) {
    read_buf_ = NULL;
    read_buf_len_ = 0;
    read_callback_ = nullptr;
  }

  read_if_ready_callback_ = nullptr;

  if (write_callback_) {
    write_buf_ = NULL;
    write_buf_len_ = 0;
    write_callback_ = nullptr;
  }

  waiting_connect_ = false;
  peer_address_.reset();
}

}  // namespace net
}  // namespace felicia