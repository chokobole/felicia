#include "felicia/core/communication/tcp_connection.h"

#include "felicia/core/lib/net/errors.h"
#include "felicia/core/lib/net/ip_address.h"
#include "felicia/core/lib/net/ip_endpoint.h"

namespace felicia {

TCPConnection::TCPConnection() = default;
TCPConnection::~TCPConnection() = default;

int TCPConnection::Setup() {
  auto server_socket = absl::make_unique<net::TCPSocket>();
  int rv = server_socket->Open(net::ADDRESS_FAMILY_IPV4);
  if (rv != net::OK) {
    return rv;
  }

  net::IPAddress address(0, 0, 0, 0);
  net::IPEndPoint endpoint(address, 9000);
  rv = server_socket->Bind(endpoint);
  if (rv != net::OK) {
    return rv;
  }

  rv = server_socket->SetReuseAddr(true);
  if (rv != net::OK) {
    return rv;
  }

  rv = server_socket->Listen(5);
  if (rv != net::OK) {
    return rv;
  }

  socket_ = std::move(server_socket);

  DoAcceptLoop();

  return net::OK;
}

int TCPConnection::Connect() {
  auto client_socket = absl::make_unique<net::TCPSocket>();
  int rv = client_socket->Open(net::ADDRESS_FAMILY_IPV4);
  if (rv != net::OK) {
    return rv;
  }

  net::IPAddress address(127, 0, 0, 1);
  net::IPEndPoint endpoint(address, 9000);
  rv = client_socket->Connect(endpoint, std::bind(&TCPConnection::OnConnect,
                                                  this, std::placeholders::_1));
  if (rv != net::OK && rv != net::ERR_IO_PENDING) {
    return rv;
  }

  socket_ = std::move(client_socket);
  if (rv == net::OK) {
    OnConnect(rv);
  }

  return net::OK;
}

void TCPConnection::DoAcceptLoop() {
  int result = net::OK;
  while (result == net::OK) {
    result = socket_->Accept(
        &accepted_socket_,
        std::bind(&TCPConnection::OnAccept, this, std::placeholders::_1));
    if (result != net::ERR_IO_PENDING) HandleAccpetResult(result);
  }
}

void TCPConnection::HandleAccpetResult(int result) {
  DCHECK_NE(result, net::ERR_IO_PENDING);

  if (result < 0) {
    LOG(ERROR) << "Error when accepting a connection: "
               << net::ErrorToString(result);
    return;
  }

  LOG(INFO) << "Accepted";
  accepted_sockets_.push_back(std::move(accepted_socket_));
}

void TCPConnection::OnAccept(int result) {
  HandleAccpetResult(result);
  if (result == net::OK) DoAcceptLoop();
}

void TCPConnection::OnConnect(int result) { LOG(INFO) << "OnConnect"; }

}  // namespace felicia