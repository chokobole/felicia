#include "felicia/core/channel/udp_client_channel.h"

#include <limits>
#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/platform/net_util.h"

namespace felicia {

UDPClientChannel::UDPClientChannel() = default;
UDPClientChannel::~UDPClientChannel() = default;

void UDPClientChannel::Connect(const ::net::IPEndPoint& ip_endpoint,
                               StatusCallback callback) {
  DCHECK(callback);
  auto client_socket = std::make_unique<::net::UDPSocket>(
      ::net::DatagramSocket::BindType::DEFAULT_BIND);
  int rv = client_socket->Open(ip_endpoint.GetFamily());
  if (rv != ::net::OK) {
    callback(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->SetMulticastLoopbackMode(true);
  if (rv != ::net::OK) {
    callback(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->AllowAddressSharingForMulticast();
  if (rv != ::net::OK) {
    callback(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  ::net::IPAddress address(0, 0, 0, 0);
  ::net::IPEndPoint endpoint(address, ip_endpoint.port());
  rv = client_socket->Bind(endpoint);
  if (rv != ::net::OK) {
    callback(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->SetReceiveBufferSize(kMaxReceiverBufferSize);
  if (rv != ::net::OK) {
    callback(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->JoinGroup(ip_endpoint.address());
  if (rv != ::net::OK) {
    callback(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  // rv = client_socket->SetMulticastInterface(
  //     net::MulticastInterfaceIndex(net::HOST_IP_ONLY_ALLOW_IPV4));
  // if (rv != ::net::OK) {
  //   callback(errors::NetworkError(::net::ErrorToString(rv)));
  //   return;
  // }

  // rv = client_socket->Connect(ip_endpoint);

  socket_ = std::move(client_socket);
  multicast_ip_endpoint_ = ip_endpoint;
  callback(Status::OK());
}

void UDPClientChannel::Write(::net::IOBuffer* buf, size_t buf_len,
                             StatusCallback callback) {
  DCHECK(callback);
  write_callback_ = callback;
  int rv = socket_->SendTo(
      buf, buf_len, multicast_ip_endpoint_,
      ::base::BindOnce(&UDPClientChannel::OnWrite, ::base::Unretained(this)));
  if (rv != ::net::ERR_IO_PENDING) {
    OnWrite(rv);
  }
}

void UDPClientChannel::Read(::net::IOBuffer* buf, size_t buf_len,
                            StatusCallback callback) {
  DCHECK(callback);
  read_callback_ = callback;
  int rv = socket_->Read(
      buf, buf_len,
      ::base::BindOnce(&UDPClientChannel::OnRead, ::base::Unretained(this)));
  if (rv != ::net::ERR_IO_PENDING) {
    OnRead(rv);
  }
}

}  // namespace felicia