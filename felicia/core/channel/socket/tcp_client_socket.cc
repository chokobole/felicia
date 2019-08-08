#include "felicia/core/channel/socket/tcp_client_socket.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

TCPClientSocket::TCPClientSocket() = default;

TCPClientSocket::TCPClientSocket(std::unique_ptr<net::TCPSocket> socket)
    : TCPSocket(std::move(socket)) {}

TCPClientSocket::~TCPClientSocket() = default;

void TCPClientSocket::Connect(const net::IPEndPoint& ip_endpoint,
                              StatusOnceCallback callback) {
  DCHECK(!socket_);
  DCHECK(connect_callback_.is_null());
  DCHECK(!callback.is_null());
  auto client_socket = std::make_unique<net::TCPSocket>(nullptr);
  int rv = client_socket->Open(ip_endpoint.GetFamily());
  if (rv != net::OK) {
    std::move(callback).Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  client_socket->SetDefaultOptionsForClient();

  rv = client_socket->Connect(
      ip_endpoint,
      base::BindOnce(&TCPClientSocket::OnConnect, base::Unretained(this)));
  if (rv != net::OK && rv != net::ERR_IO_PENDING) {
    std::move(callback).Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  connect_callback_ = std::move(callback);
  socket_ = std::move(client_socket);

  if (rv == net::OK) {
    std::move(connect_callback_).Run(Status::OK());
  }
}

bool TCPClientSocket::IsClient() const { return true; }

bool TCPClientSocket::IsConnected() const {
  return socket_ && socket_->IsConnected();
}

void TCPClientSocket::Write(scoped_refptr<net::IOBuffer> buffer, int size,
                            StatusOnceCallback callback) {
  WriteRepeating(
      buffer, size, std::move(callback),
      base::BindRepeating(&TCPClientSocket::OnWrite, base::Unretained(this)));
}

void TCPClientSocket::Read(scoped_refptr<net::GrowableIOBuffer> buffer,
                           int size, StatusOnceCallback callback) {
  ReadRepeating(
      buffer, size, std::move(callback),
      base::BindRepeating(&TCPClientSocket::OnRead, base::Unretained(this)));
}

void TCPClientSocket::OnWriteCheckingReset(int result) {
  if (result == net::ERR_CONNECTION_RESET) {
    socket_.reset();
  }
  Socket::OnWrite(result);
}

void TCPClientSocket::OnReadCheckingClosed(int result) {
  if (result == 0) {
    result = net::ERR_CONNECTION_CLOSED;
    socket_.reset();
  }
  Socket::OnRead(result);
}

}  // namespace felicia