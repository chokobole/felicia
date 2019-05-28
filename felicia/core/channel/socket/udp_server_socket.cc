#include "felicia/core/channel/socket/udp_server_socket.h"

#include "third_party/chromium/base/rand_util.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/net/net_util.h"

namespace felicia {

UDPServerSocket::UDPServerSocket() = default;
UDPServerSocket::~UDPServerSocket() = default;

bool UDPServerSocket::IsServer() const { return true; }

StatusOr<ChannelDef> UDPServerSocket::Bind() {
  auto server_socket = std::make_unique<::net::UDPSocket>(
      ::net::DatagramSocket::BindType::DEFAULT_BIND);

  int rv = server_socket->Open(::net::ADDRESS_FAMILY_IPV4);
  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  uint16_t port = net::PickRandomPort(false);
  ::net::IPAddress address(0, 0, 0, 0);
  ::net::IPEndPoint server_endpoint(address, port);
  rv = server_socket->Bind(server_endpoint);
  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
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
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  socket_ = std::move(server_socket);
  multicast_ip_endpoint_ =
      ::net::IPEndPoint(multicast_address, net::PickRandomPort(false));

  return ToChannelDef(multicast_ip_endpoint_, ChannelDef::UDP);
}

void UDPServerSocket::Write(char* buffer, int size,
                            StatusOnceCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  write_callback_ = std::move(callback);
  int to_write = size;
  int written = 0;
  while (to_write > 0) {
    scoped_refptr<::net::IOBufferWithSize> write_buffer =
        base::MakeRefCounted<::net::IOBufferWithSize>(to_write);
    memcpy(write_buffer->data(), buffer + written, to_write);
    int rv = socket_->SendTo(
        write_buffer.get(), write_buffer->size(), multicast_ip_endpoint_,
        ::base::BindOnce(&UDPServerSocket::OnWrite, ::base::Unretained(this)));

    if (rv == ::net::ERR_IO_PENDING) break;

    if (rv >= 0) {
      to_write -= rv;
      written += rv;
    }

    if (to_write == 0 || rv <= 0) {
      OnWrite(rv);
      break;
    }
  }
}

void UDPServerSocket::Read(char* buffer, int size,
                           StatusOnceCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  read_callback_ = std::move(callback);
  int to_read = size;
  int read = 0;
  while (to_read > 0) {
    scoped_refptr<::net::IOBufferWithSize> read_buffer =
        base::MakeRefCounted<::net::IOBufferWithSize>(to_read);
    int rv = socket_->RecvFrom(
        read_buffer.get(), read_buffer->size(), &recv_from_ip_endpoint_,
        ::base::BindOnce(&UDPServerSocket::OnReadAsync,
                         ::base::Unretained(this), buffer + read, read_buffer));

    if (rv == ::net::ERR_IO_PENDING) break;

    if (rv > 0) {
      memcpy(buffer + read, read_buffer->data(), rv);
      to_read -= rv;
      read += rv;
    }

    if (to_read == 0 || rv <= 0) {
      OnRead(rv);
      break;
    }
  }
}

}  // namespace felicia