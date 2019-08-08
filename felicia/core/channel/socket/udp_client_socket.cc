#include "felicia/core/channel/socket/udp_client_socket.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

UDPClientSocket::UDPClientSocket() = default;
UDPClientSocket::~UDPClientSocket() = default;

void UDPClientSocket::Connect(const net::IPEndPoint& ip_endpoint,
                              StatusOnceCallback callback) {
  DCHECK(!callback.is_null());
  auto client_socket = std::make_unique<net::UDPSocket>(
      net::DatagramSocket::BindType::DEFAULT_BIND);
  int rv = client_socket->Open(ip_endpoint.GetFamily());
  if (rv != net::OK) {
    std::move(callback).Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->SetMulticastLoopbackMode(true);
  if (rv != net::OK) {
    std::move(callback).Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->AllowAddressSharingForMulticast();
  if (rv != net::OK) {
    std::move(callback).Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  net::IPAddress address(0, 0, 0, 0);
  net::IPEndPoint endpoint(address, ip_endpoint.port());
  rv = client_socket->Bind(endpoint);
  if (rv != net::OK) {
    std::move(callback).Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->JoinGroup(ip_endpoint.address());
  if (rv != net::OK) {
    std::move(callback).Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  socket_ = std::move(client_socket);
  multicast_ip_endpoint_ = ip_endpoint;
  std::move(callback).Run(Status::OK());
}

bool UDPClientSocket::IsClient() const { return true; }

void UDPClientSocket::Write(scoped_refptr<net::IOBuffer> buffer, int size,
                            StatusOnceCallback callback) {
  WriteRepeating(
      buffer, size, std::move(callback),
      base::BindRepeating(&UDPClientSocket::OnWrite, base::Unretained(this)));
}

void UDPClientSocket::Read(scoped_refptr<net::GrowableIOBuffer> buffer,
                           int size, StatusOnceCallback callback) {
  ReadRepeating(
      buffer, size, std::move(callback),
      base::BindRepeating(&UDPClientSocket::OnRead, base::Unretained(this)));
}

}  // namespace felicia