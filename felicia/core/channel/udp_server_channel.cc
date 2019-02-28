#include "felicia/core/channel/udp_server_channel.h"

#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/rand_util.h"
#include "third_party/chromium/base/strings/strcat.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/platform/net_util.h"

namespace felicia {

UDPServerChannel::UDPServerChannel() = default;
UDPServerChannel::~UDPServerChannel() = default;

void UDPServerChannel::Bind(const NodeInfo& node_info,
                            StatusOrIPEndPointCallback callback) {
  DCHECK(callback);
  auto server_socket = std::make_unique<::net::UDPSocket>(
      ::net::DatagramSocket::BindType::DEFAULT_BIND);

  ::net::IPAddress ip;
  if (!ip.AssignFromIPLiteral(node_info.ip_endpoint().ip())) {
    std::move(callback).Run(errors::InvalidArgument(::base::StrCat(
        {"IP Address is not valid ", node_info.ip_endpoint().ip()})));
    return;
  }

  int rv;
  if (ip.IsIPv4()) {
    rv = server_socket->Open(::net::AddressFamily::ADDRESS_FAMILY_IPV4);
  } else {
    rv = server_socket->Open(::net::AddressFamily::ADDRESS_FAMILY_IPV6);
  }
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  uint16_t port = node_info.ip_endpoint().port();
  if (port == 0) {
    port = net::PickRandomPort(false);
  }
  ::net::IPAddress address(0, 0, 0, 0);
  ::net::IPEndPoint endpoint(address, port);
  rv = server_socket->Bind(endpoint);
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  ::net::IPAddress multicast_address;
  for (int i = 0; i < 1000; i++) {
    multicast_address = ::net::IPAddress(239, ::base::RandGenerator(255),
                                         ::base::RandGenerator(255),
                                         ::base::RandGenerator(255));
    rv = server_socket->JoinGroup(multicast_address);
    if (rv == ::net::OK) {
      server_socket->LeaveGroup(multicast_address);
      break;
    }
  }
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  socket_ = std::move(server_socket);
  multicast_ip_endpoint_ =
      ::net::IPEndPoint(multicast_address, net::PickRandomPort(false));
  std::move(callback).Run(multicast_ip_endpoint_);
}

void UDPServerChannel::Write(::net::IOBufferWithSize* buffer,
                             StatusCallback callback) {
  DCHECK(!callback.is_null());
  write_callback_ = std::move(callback);
  int rv = socket_->SendTo(
      buffer, buffer->size(), multicast_ip_endpoint_,
      ::base::BindOnce(&UDPServerChannel::OnWrite, ::base::Unretained(this)));
  if (rv != ::net::ERR_IO_PENDING) {
    OnWrite(rv);
  }
}

void UDPServerChannel::Read(::net::IOBufferWithSize* buffer,
                            StatusCallback callback) {
  DCHECK(!callback.is_null());
  read_callback_ = std::move(callback);
  int rv = socket_->RecvFrom(
      buffer, buffer->size(), &recv_from_ip_endpoint_,
      ::base::BindOnce(&UDPServerChannel::OnRead, ::base::Unretained(this)));
  if (rv != ::net::ERR_IO_PENDING) {
    OnRead(rv);
  }
}

}  // namespace felicia