#include "felicia/core/channel/socket/tcp_server_socket.h"

#include "felicia/core/channel/socket/tcp_client_socket.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/net/net_util.h"

namespace felicia {

TCPServerSocket::TCPServerSocket() : broadcaster_(&accepted_sockets_) {}
TCPServerSocket::~TCPServerSocket() = default;

const std::vector<std::unique_ptr<StreamSocket>>&
TCPServerSocket::accepted_sockets() const {
  return accepted_sockets_;
}

bool TCPServerSocket::IsServer() const { return true; }

StatusOr<ChannelDef> TCPServerSocket::Listen() {
  auto server_socket = std::make_unique<net::TCPSocket>(nullptr);

  int rv = server_socket->Open(net::ADDRESS_FAMILY_IPV4);
  if (rv != net::OK) {
    return errors::NetworkError(net::ErrorToString(rv));
  }

  uint16_t port = PickRandomPort(true);
  net::IPAddress address(0, 0, 0, 0);
  net::IPEndPoint server_endpoint(address, port);
  rv = server_socket->Bind(server_endpoint);
  if (rv != net::OK) {
    return errors::NetworkError(net::ErrorToString(rv));
  }

  rv = server_socket->SetDefaultOptionsForServer();
  if (rv != net::OK) {
    return errors::NetworkError(net::ErrorToString(rv));
  }

  rv = server_socket->Listen(5);
  if (rv != net::OK) {
    return errors::NetworkError(net::ErrorToString(rv));
  }

  socket_ = std::move(server_socket);

  return ToChannelDef(
      net::IPEndPoint(HostIPAddress(HOST_IP_ONLY_ALLOW_IPV4), port),
      ChannelDef::CHANNEL_TYPE_TCP);
}

void TCPServerSocket::AcceptLoop(AcceptCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(accept_callback_.is_null() &&
         accept_once_intercept_callback_.is_null());
  accept_callback_ = callback;
  DoAcceptLoop();
}

void TCPServerSocket::AcceptOnceIntercept(
    AcceptOnceInterceptCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(accept_callback_.is_null() &&
         accept_once_intercept_callback_.is_null());
  accept_once_intercept_callback_ = std::move(callback);
  DoAccept();
}

void TCPServerSocket::AddSocket(std::unique_ptr<net::TCPSocket> socket) {
  accepted_sockets_.push_back(
      std::make_unique<TCPClientSocket>(std::move(socket)));
}

void TCPServerSocket::AddSocket(std::unique_ptr<TCPClientSocket> socket) {
  accepted_sockets_.push_back(std::move(socket));
}

#if !defined(FEL_NO_SSL)
void TCPServerSocket::AddSocket(std::unique_ptr<SSLServerSocket> socket) {
  accepted_sockets_.push_back(std::move(socket));
}
#endif  // !defined(FEL_NO_SSL)

bool TCPServerSocket::IsConnected() const {
  for (auto& accepted_socket : accepted_sockets_) {
    if (accepted_socket->IsConnected()) return true;
  }
  return false;
}

void TCPServerSocket::WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                                 StatusOnceCallback callback) {
  DCHECK(write_callback_.is_null());
  write_callback_ = std::move(callback);
  broadcaster_.Broadcast(
      buffer, size,
      base::BindOnce(&TCPServerSocket::OnWrite, base::Unretained(this)));
}

void TCPServerSocket::ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer,
                                int size, StatusOnceCallback callback) {
  NOTREACHED() << "You read data from ServerSocket, if you need, please use "
                  "TCPServerSokcet::AcceptOnceIntercept.";
}

int TCPServerSocket::DoAccept() {
  int result = socket_->Accept(
      &accepted_socket_, &accepted_endpoint_,
      base::BindOnce(&TCPServerSocket::OnAccept, base::Unretained(this)));
  if (result != net::ERR_IO_PENDING) HandleAccpetResult(result);
  return result;
}

void TCPServerSocket::DoAcceptLoop() {
  int result = net::OK;
  while (result == net::OK) {
    result = DoAccept();
  }
}

void TCPServerSocket::HandleAccpetResult(int result) {
  DCHECK_NE(result, net::ERR_IO_PENDING);

  if (result < 0) {
    if (accept_once_intercept_callback_) {
      std::move(accept_once_intercept_callback_)
          .Run(errors::NetworkError(net::ErrorToString(result)));
    } else if (accept_callback_) {
      accept_callback_.Run(errors::NetworkError(net::ErrorToString(result)));
    }
    return;
  }

  if (accept_once_intercept_callback_) {
    std::move(accept_once_intercept_callback_).Run(std::move(accepted_socket_));
  } else {
    accepted_sockets_.push_back(
        std::make_unique<TCPClientSocket>(std::move(accepted_socket_)));
    if (accept_callback_) accept_callback_.Run(Status::OK());
  }
}

void TCPServerSocket::OnAccept(int result) {
  HandleAccpetResult(result);
  if (accept_callback_) DoAcceptLoop();
}

void TCPServerSocket::OnWrite(Status s) {
  std::move(write_callback_).Run(std::move(s));
}

}  // namespace felicia