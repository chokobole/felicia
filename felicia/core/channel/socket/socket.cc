#include "felicia/core/channel/socket/socket.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

Socket::Socket() = default;
Socket::~Socket() = default;

bool Socket::IsSocket() const { return true; }

bool Socket::IsClient() const { return false; }
bool Socket::IsServer() const { return false; }
bool Socket::IsStreamSocket() const { return false; }
bool Socket::IsDatagramSocket() const { return false; }

bool Socket::IsTCPSocket() const { return false; }
bool Socket::IsUDPSocket() const { return false; }
bool Socket::IsWebSocket() const { return false; }
bool Socket::IsSSLSocket() const { return false; }
#if defined(OS_POSIX)
bool Socket::IsUnixDomainSocket() const { return false; }
#endif

StreamSocket* Socket::ToStreamSocket() {
  DCHECK(IsStreamSocket());
  return reinterpret_cast<StreamSocket*>(this);
}

DatagramSocket* Socket::ToDatagramSocket() {
  DCHECK(IsDatagramSocket());
  return reinterpret_cast<DatagramSocket*>(this);
}

TCPSocket* Socket::ToTCPSocket() {
  DCHECK(IsTCPSocket());
  return reinterpret_cast<TCPSocket*>(this);
}

UDPSocket* Socket::ToUDPSocket() {
  DCHECK(IsUDPSocket());
  return reinterpret_cast<UDPSocket*>(this);
}

WebSocket* Socket::ToWebSocket() {
  DCHECK(IsWebSocket());
  return reinterpret_cast<WebSocket*>(this);
}

SSLSocket* Socket::ToSSLSocket() {
  DCHECK(IsSSLSocket());
  return reinterpret_cast<SSLSocket*>(this);
}

#if defined(OS_POSIX)
UnixDomainSocket* Socket::ToUnixDomainSocket() {
  DCHECK(IsUnixDomainSocket());
  return reinterpret_cast<UnixDomainSocket*>(this);
}
#endif

void Socket::WriteRepeating(
    scoped_refptr<net::IOBuffer> buffer, int size, StatusOnceCallback callback,
    net::CompletionRepeatingCallback on_write_callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  write_callback_ = std::move(callback);
  scoped_refptr<net::DrainableIOBuffer> write_buffer =
      base::MakeRefCounted<net::DrainableIOBuffer>(buffer,
                                                   static_cast<size_t>(size));
  while (write_buffer->BytesRemaining() > 0) {
    int rv = Write(write_buffer.get(), write_buffer->BytesRemaining(),
                   on_write_callback);

    if (rv == net::ERR_IO_PENDING) break;

    if (rv > 0) {
      write_buffer->DidConsume(rv);
    }

    if (write_buffer->BytesRemaining() == 0 || rv <= 0) {
      on_write_callback.Run(rv);
      break;
    }
  }
}

void Socket::ReadRepeating(scoped_refptr<net::GrowableIOBuffer> buffer,
                           int size, StatusOnceCallback callback,
                           net::CompletionRepeatingCallback on_read_callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  read_callback_ = std::move(callback);
  int to_read = size;
  while (to_read > 0) {
    int rv = Read(buffer.get(), to_read, on_read_callback);

    if (rv == net::ERR_IO_PENDING) break;

    if (rv > 0) {
      buffer->set_offset(buffer->offset() + rv);
      to_read -= rv;
    }

    if (to_read == 0 || rv <= 0) {
      on_read_callback.Run(rv);
      break;
    }
  }
}

void Socket::OnConnect(int result) {
  CallbackWithStatus(std::move(connect_callback_), result);
}

void Socket::OnWrite(int result) {
  CallbackWithStatus(std::move(write_callback_), result);
}

void Socket::OnRead(int result) {
  CallbackWithStatus(std::move(read_callback_), result);
}

// static
void Socket::CallbackWithStatus(StatusOnceCallback callback, int result) {
  if (result >= 0) {
    std::move(callback).Run(Status::OK());
  } else {
    std::move(callback).Run(errors::NetworkError(net::ErrorToString(result)));
  }
}

}  // namespace felicia