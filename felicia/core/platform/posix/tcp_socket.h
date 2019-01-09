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

#ifndef FELICIA_CORE_PLATFORM_POSIX_TCP_SOCKET_H_
#define FELICIA_CORE_PLATFORM_POSIX_TCP_SOCKET_H_

#include "felicia/core/lib/io/buffer.h"
#include "felicia/core/platform/message_pump.h"

namespace felicia {
namespace net {

class EXPORT TCPSocket : public TCPSocketBase, public MessagePump::FdWatcher {
 public:
  TCPSocket();
  virtual ~TCPSocket();
  // Open a Socket. Returns a net error code.
  int Open(AddressFamily family) MUST_USE_RESULT;

  // Takes ownership of |socket|, which is known to already be connected to the
  // given peer address.
  int AdoptConnectedSocket(SocketDescriptor socket,
                           const IPEndPoint& peer_address);
  // Takes ownership of |socket|, which may or may not be open, bound, or
  // listening. The caller must determine the state of the socket based on its
  // provenance and act accordingly. The socket may have connections waiting
  // to be accepted, but must not be actually connected.
  int AdoptUnconnectedSocket(SocketDescriptor socket);

  // Releases ownership of |socket_fd_| to caller.
  SocketDescriptor ReleaseConnectedSocket();

  // Closes a Socket. Returns a net error code.
  int Close() MUST_USE_RESULT;
  // Listens a socket. Returns a net error code.
  int Listen(int backlog) MUST_USE_RESULT;
  // Accepts incoming connection.
  // Returns a net error code.
  int Accept(std::unique_ptr<TCPSocket>* socket,
             CompletionOnceCallback callback) MUST_USE_RESULT;

  // Connects socket. On non-ERR_IO_PENDING error, sets errno and returns a net
  // error code. On ERR_IO_PENDING, |callback| is called with a net error code,
  // not errno, though errno is set if connect event happens with error.
  int Connect(const IPEndPoint& endpoint,
              CompletionOnceCallback callback) MUST_USE_RESULT;
  bool IsConnected() const;
  bool IsConnectedAndIdle() const;

  // Multiple outstanding requests of the same type are not supported.
  // Full duplex mode (reading and writing at the same time) is supported.
  // On error which is not ERR_IO_PENDING, sets errno and returns a net error
  // code. On ERR_IO_PENDING, |callback| is called with a net error code, not
  // errno, though errno is set if read or write events happen with error.
  size_t Read(Buffer* buf, size_t buf_len,
              CompletionOnceCallback callback) override;
  // Reads up to |buf_len| bytes into |buf| without blocking. If read is to
  // be retried later, |callback| will be invoked when data is ready for
  // reading. This method doesn't hold on to |buf|.
  int ReadIfReady(Buffer* buf, int buf_len, CompletionOnceCallback callback);
  int CancelReadIfReady();
  size_t Write(Buffer* buf, size_t buf_len,
               CompletionOnceCallback callback) override;

  // Waits for next write event. This is called by TCPSocket for TCP
  // fastopen after sending first data. Returns ERR_IO_PENDING if it starts
  // waiting for write event successfully. Otherwise, returns a net error code.
  // It must not be called after Write() because Write() calls it internally.
  int WaitForWrite(Buffer* buf, int buf_len, CompletionOnceCallback callback);

 private:
  // felicia::MessagePump::FdWatcher methods.
  void OnFileCanReadWithoutBlocking(int fd) override;
  void OnFileCanWriteWithoutBlocking(int fd) override;

  int DoAccept(std::unique_ptr<TCPSocket>* socket);
  void AcceptCompleted();

  int DoConnect();
  void ConnectCompleted();

  int DoRead(Buffer* buf, int buf_len);
  void RetryRead(int rv);
  void ReadCompleted();

  int DoWrite(Buffer* buf, int buf_len);
  void WriteCompleted();

  void StopWatchingAndCleanUp();

  MessagePump::FdWatchController accept_socket_watcher_;
  std::unique_ptr<TCPSocket>* accept_socket_;
  CompletionOnceCallback accept_callback_;

  // Non-null when a Read() is in progress.
  MessagePump::FdWatchController read_socket_watcher_;
  std::shared_ptr<Buffer> read_buf_;
  int read_buf_len_;
  CompletionOnceCallback read_callback_;

  // Non-null when a ReadIfReady() is in progress.
  CompletionOnceCallback read_if_ready_callback_;

  MessagePump::FdWatchController write_socket_watcher_;
  std::shared_ptr<Buffer> write_buf_;
  int write_buf_len_;
  // External callback; called when write or connect is complete.
  CompletionOnceCallback write_callback_;

  // A connect operation is pending. In this case, |write_callback_| needs to be
  // called when connect is complete.
  bool waiting_connect_;

  DISALLOW_COPY_AND_ASSIGN(TCPSocket);
};

}  // namespace net
}  // namespace felicia

#endif  // FELICIA_CORE_PLATFORM_POSIX_TCP_SOCKET_H_
