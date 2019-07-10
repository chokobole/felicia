#include "felicia/core/channel/socket/tcp_server_socket.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/net/net_util.h"

namespace felicia {

TCPSocketInterface::TCPSocketInterface(std::unique_ptr<::net::TCPSocket> socket)
    : socket_(std::move(socket)) {}

TCPSocketInterface::TCPSocketInterface(TCPSocketInterface&& other)
    : socket_(std::move(other.socket_)) {}

void TCPSocketInterface::operator=(TCPSocketInterface&& other) {
  socket_ = std::move(other.socket_);
}

bool TCPSocketInterface::IsConnected() { return socket_->IsConnected(); }

int TCPSocketInterface::Write(::net::IOBuffer* buf, int buf_len,
                              ::net::CompletionOnceCallback callback) {
  return socket_->Write(buf, buf_len, std::move(callback),
                        ::net::DefineNetworkTrafficAnnotation(
                            "TCPSocketInterface", "Send Message"));
}

void TCPSocketInterface::Close() { return socket_->Close(); }

TCPServerSocket::TCPServerSocket() : broadcaster_(&accepted_sockets_) {}
TCPServerSocket::~TCPServerSocket() = default;

const std::vector<std::unique_ptr<TCPSocketInterface::SocketInterface>>&
TCPServerSocket::accepted_sockets() const {
  return accepted_sockets_;
}

bool TCPServerSocket::IsServer() const { return true; }

bool TCPServerSocket::IsConnected() const {
  for (auto& accepted_socket : accepted_sockets_) {
    if (accepted_socket->IsConnected()) return true;
  }
  return false;
}

StatusOr<ChannelDef> TCPServerSocket::Listen() {
  auto server_socket = std::make_unique<::net::TCPSocket>(nullptr);

  int rv = server_socket->Open(::net::ADDRESS_FAMILY_IPV4);
  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  uint16_t port = net::PickRandomPort(true);
  ::net::IPAddress address(0, 0, 0, 0);
  ::net::IPEndPoint server_endpoint(address, port);
  rv = server_socket->Bind(server_endpoint);
  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  rv = server_socket->SetDefaultOptionsForServer();
  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  rv = server_socket->Listen(5);
  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  socket_ = std::move(server_socket);

  return ToChannelDef(
      ::net::IPEndPoint(net::HostIPAddress(net::HOST_IP_ONLY_ALLOW_IPV4), port),
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

void TCPServerSocket::AddSocket(std::unique_ptr<::net::TCPSocket> socket) {
  accepted_sockets_.push_back(
      std::make_unique<TCPSocketInterface>(std::move(socket)));
}

void TCPServerSocket::Write(scoped_refptr<::net::IOBuffer> buffer, int size,
                            StatusOnceCallback callback) {
  DCHECK(write_callback_.is_null());
  write_callback_ = std::move(callback);
  broadcaster_.Broadcast(
      buffer, size,
      ::base::BindOnce(&TCPServerSocket::OnWrite, ::base::Unretained(this)));
}

void TCPServerSocket::Read(scoped_refptr<::net::GrowableIOBuffer> buffer,
                           int size, StatusOnceCallback callback) {
  NOTREACHED() << "You read data from ServerSocket, if you need, please use "
                  "TCPServerSokcet::AcceptOnceIntercept.";
}

int TCPServerSocket::DoAccept() {
  int result = socket_->Accept(
      &accepted_socket_, &accepted_endpoint_,
      ::base::BindOnce(&TCPServerSocket::OnAccept, ::base::Unretained(this)));
  if (result != ::net::ERR_IO_PENDING) HandleAccpetResult(result);
  return result;
}

void TCPServerSocket::DoAcceptLoop() {
  int result = ::net::OK;
  while (result == ::net::OK) {
    result = DoAccept();
  }
}

void TCPServerSocket::HandleAccpetResult(int result) {
  DCHECK_NE(result, ::net::ERR_IO_PENDING);

  if (result < 0) {
    if (accept_once_intercept_callback_) {
      std::move(accept_once_intercept_callback_)
          .Run(errors::NetworkError(::net::ErrorToString(result)));
    } else if (accept_callback_) {
      accept_callback_.Run(errors::NetworkError(::net::ErrorToString(result)));
    }
    return;
  }

  if (accept_once_intercept_callback_) {
    std::move(accept_once_intercept_callback_).Run(std::move(accepted_socket_));
  } else {
    accepted_sockets_.push_back(
        std::make_unique<TCPSocketInterface>(std::move(accepted_socket_)));
    if (accept_callback_) accept_callback_.Run(Status::OK());
  }
}

void TCPServerSocket::OnAccept(int result) {
  HandleAccpetResult(result);
  if (accept_callback_) DoAcceptLoop();
}

void TCPServerSocket::OnWrite(const Status& s) {
  std::move(write_callback_).Run(s);
}

}  // namespace felicia