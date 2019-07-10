#include "felicia/core/channel/socket/unix_domain_client_socket.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

UnixDomainClientSocket::UnixDomainClientSocket() = default;
UnixDomainClientSocket::~UnixDomainClientSocket() = default;

int UnixDomainClientSocket::socket_fd() const {
  if (socket_) {
    return socket_->socket_fd();
  }
  return ::net::kInvalidSocket;
}

void UnixDomainClientSocket::set_socket(
    std::unique_ptr<::net::SocketPosix> socket) {
  socket_ = std::move(socket);
}

bool UnixDomainClientSocket::IsClient() const { return true; }

void UnixDomainClientSocket::Connect(const ::net::UDSEndPoint& uds_endpoint,
                                     StatusOnceCallback callback) {
  DCHECK(!socket_);
  DCHECK(!callback.is_null());

  ::net::SockaddrStorage address;
  if (!uds_endpoint.ToSockAddrStorage(&address)) {
    std::move(callback).Run(
        errors::NetworkError(::net::ErrorToString(::net::ERR_ADDRESS_INVALID)));
    return;
  }

  auto client_socket = std::make_unique<::net::SocketPosix>();

  int rv = client_socket->Open(AF_UNIX);
  DCHECK_NE(::net::ERR_IO_PENDING, rv);
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->Connect(
      address, ::base::BindOnce(&UnixDomainClientSocket::OnConnect,
                                ::base::Unretained(this)));
  if (rv != ::net::OK && rv != ::net::ERR_IO_PENDING) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  connect_callback_ = std::move(callback);
  socket_ = std::move(client_socket);

  if (rv == ::net::OK) {
    std::move(connect_callback_).Run(Status::OK());
  }
}

void UnixDomainClientSocket::Write(scoped_refptr<::net::IOBuffer> buffer,
                                   int size, StatusOnceCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  write_callback_ = std::move(callback);
  scoped_refptr<::net::DrainableIOBuffer> write_buffer =
      ::base::MakeRefCounted<::net::DrainableIOBuffer>(
          buffer, static_cast<size_t>(size));
  while (write_buffer->BytesRemaining() > 0) {
    int rv = socket_->Write(write_buffer.get(), write_buffer->BytesRemaining(),
                            ::base::BindOnce(&UnixDomainClientSocket::OnWrite,
                                             ::base::Unretained(this)),
                            ::net::DefineNetworkTrafficAnnotation(
                                "unix_domain_client_socket", "Send Message"));

    if (rv == ::net::ERR_IO_PENDING) break;

    if (rv > 0) {
      write_buffer->DidConsume(rv);
    }

    if (write_buffer->BytesRemaining() == 0 || rv <= 0) {
      OnWrite(rv);
      break;
    }
  }
}

void UnixDomainClientSocket::Read(scoped_refptr<::net::GrowableIOBuffer> buffer,
                                  int size, StatusOnceCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  read_callback_ = std::move(callback);
  int to_read = size;
  while (to_read > 0) {
    int rv = socket_->Read(buffer.get(), to_read,
                           ::base::BindOnce(&UnixDomainClientSocket::OnRead,
                                            ::base::Unretained(this)));

    if (rv == ::net::ERR_IO_PENDING) break;

    if (rv > 0) {
      buffer->set_offset(buffer->offset() + rv);
      to_read -= rv;
    }

    if (to_read == 0 || rv <= 0) {
      OnRead(rv);
      break;
    }
  }
}

void UnixDomainClientSocket::OnConnect(int result) {
  CallbackWithStatus(std::move(connect_callback_), result);
}

void UnixDomainClientSocket::OnWrite(int result) {
  CallbackWithStatus(std::move(write_callback_), result);
}

void UnixDomainClientSocket::OnRead(int result) {
  CallbackWithStatus(std::move(read_callback_), result);
}

}  // namespace felicia