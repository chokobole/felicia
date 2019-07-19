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

bool UnixDomainClientSocket::IsClient() const { return true; }

bool UnixDomainClientSocket::IsConnected() const {
  return socket_ && socket_->IsConnected();
}

void UnixDomainClientSocket::Write(scoped_refptr<::net::IOBuffer> buffer,
                                   int size, StatusOnceCallback callback) {
  WriteRepeating(buffer, size, std::move(callback),
                 ::base::BindRepeating(&UnixDomainClientSocket::OnWrite,
                                       ::base::Unretained(this)));
}

void UnixDomainClientSocket::Read(scoped_refptr<::net::GrowableIOBuffer> buffer,
                                  int size, StatusOnceCallback callback) {
  ReadRepeating(buffer, size, std::move(callback),
                ::base::BindRepeating(&UnixDomainClientSocket::OnRead,
                                      ::base::Unretained(this)));
}

}  // namespace felicia