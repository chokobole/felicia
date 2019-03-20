#include "felicia/core/channel/udp_client_channel.h"

#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/net/net_util.h"

namespace felicia {

UDPClientChannel::UDPClientChannel() = default;
UDPClientChannel::~UDPClientChannel() = default;

bool UDPClientChannel::IsClient() const { return true; }

void UDPClientChannel::Connect(const ::net::IPEndPoint& ip_endpoint,
                               StatusCallback callback) {
  DCHECK(!callback.is_null());
  auto client_socket = std::make_unique<::net::UDPSocket>(
      ::net::DatagramSocket::BindType::DEFAULT_BIND);
  int rv = client_socket->Open(ip_endpoint.GetFamily());
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->SetMulticastLoopbackMode(true);
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->AllowAddressSharingForMulticast();
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  ::net::IPAddress address(0, 0, 0, 0);
  ::net::IPEndPoint endpoint(address, ip_endpoint.port());
  rv = client_socket->Bind(endpoint);
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->SetReceiveBufferSize(
      ChannelBase::GetMaxReceiveBufferSize());
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->JoinGroup(ip_endpoint.address());
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  socket_ = std::move(client_socket);
  multicast_ip_endpoint_ = ip_endpoint;
  std::move(callback).Run(Status::OK());
}

void UDPClientChannel::Write(::net::IOBufferWithSize* buffer,
                             StatusCallback callback) {
  DCHECK(!callback.is_null());
  write_callback_ = std::move(callback);
  int rv = socket_->SendTo(
      buffer, buffer->size(), multicast_ip_endpoint_,
      ::base::BindOnce(&UDPClientChannel::OnWrite, ::base::Unretained(this)));
  if (rv != ::net::ERR_IO_PENDING) {
    OnWrite(rv);
  }
}

void UDPClientChannel::Read(::net::IOBufferWithSize* buffer,
                            StatusCallback callback) {
  DCHECK(!callback.is_null());
  read_callback_ = std::move(callback);
  int rv = socket_->Read(
      buffer, buffer->size(),
      ::base::BindOnce(&UDPClientChannel::OnRead, ::base::Unretained(this)));
  if (rv != ::net::ERR_IO_PENDING) {
    OnRead(rv);
  }
}

}  // namespace felicia