#include "felicia/core/channel/socket/udp_server_socket.h"

#include "third_party/chromium/base/rand_util.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/net/net_util.h"

namespace felicia {

UDPServerSocket::UDPServerSocket() = default;
UDPServerSocket::~UDPServerSocket() = default;

bool UDPServerSocket::IsServer() const { return true; }

StatusOr<ChannelDef> UDPServerSocket::Bind() {
  auto server_socket = std::make_unique<net::UDPSocket>(
      net::DatagramSocket::BindType::DEFAULT_BIND);

  int rv = server_socket->Open(net::ADDRESS_FAMILY_IPV4);
  if (rv != net::OK) {
    return errors::NetworkError(net::ErrorToString(rv));
  }

  uint16_t port = PickRandomPort(false);
  net::IPAddress address(0, 0, 0, 0);
  net::IPEndPoint server_endpoint(address, port);
  rv = server_socket->Bind(server_endpoint);
  if (rv != net::OK) {
    return errors::NetworkError(net::ErrorToString(rv));
  }

  net::IPAddress multicast_address;
  for (int i = 0; i < 1000; i++) {
    multicast_address =
        net::IPAddress(239, base::RandGenerator(255), base::RandGenerator(255),
                       base::RandGenerator(255));
    rv = server_socket->JoinGroup(multicast_address);
    if (rv == net::OK) {
      server_socket->LeaveGroup(multicast_address);
      break;
    }
  }
  if (rv != net::OK) {
    return errors::NetworkError(net::ErrorToString(rv));
  }

  socket_ = std::move(server_socket);
  multicast_ip_endpoint_ =
      net::IPEndPoint(multicast_address, PickRandomPort(false));

  return ToChannelDef(multicast_ip_endpoint_, ChannelDef::CHANNEL_TYPE_UDP);
}

void UDPServerSocket::WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                                 StatusOnceCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  write_callback_ = std::move(callback);
  scoped_refptr<net::DrainableIOBuffer> write_buffer =
      base::MakeRefCounted<net::DrainableIOBuffer>(buffer,
                                                   static_cast<size_t>(size));
  while (write_buffer->BytesRemaining() > 0) {
    int rv = socket_->SendTo(
        write_buffer.get(), write_buffer->BytesRemaining(),
        multicast_ip_endpoint_,
        base::BindOnce(&UDPServerSocket::OnWrite, base::Unretained(this)));

    if (rv == net::ERR_IO_PENDING) break;

    if (rv >= 0) {
      write_buffer->DidConsume(rv);
    }

    if (write_buffer->BytesRemaining() == 0 || rv <= 0) {
      OnWrite(rv);
      break;
    }
  }
}

void UDPServerSocket::ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer,
                                int size, StatusOnceCallback callback) {
  NOTREACHED() << "You read data from ServerSocket";
}

}  // namespace felicia