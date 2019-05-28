#include "felicia/core/channel/socket/udp_client_socket.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

UDPClientSocket::UDPClientSocket() = default;
UDPClientSocket::~UDPClientSocket() = default;

bool UDPClientSocket::IsClient() const { return true; }

void UDPClientSocket::Connect(const ::net::IPEndPoint& ip_endpoint,
                              StatusOnceCallback callback) {
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

  rv = client_socket->JoinGroup(ip_endpoint.address());
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  socket_ = std::move(client_socket);
  multicast_ip_endpoint_ = ip_endpoint;
  std::move(callback).Run(Status::OK());
}

void UDPClientSocket::Write(char* buffer, int size,
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
        ::base::BindOnce(&UDPClientSocket::OnWrite, ::base::Unretained(this)));

    if (rv == ::net::ERR_IO_PENDING) break;

    if (rv > 0) {
      to_write -= rv;
      written += rv;
    }

    if (to_write == 0 || rv <= 0) {
      OnWrite(rv);
      break;
    }
  }
}

void UDPClientSocket::Read(char* buffer, int size,
                           StatusOnceCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  read_callback_ = std::move(callback);

  int to_read = size;
  int read = 0;
  while (to_read > 0) {
    scoped_refptr<::net::IOBufferWithSize> read_buffer =
        base::MakeRefCounted<::net::IOBufferWithSize>(to_read);
    int rv = socket_->Read(
        read_buffer.get(), read_buffer->size(),
        ::base::BindOnce(&UDPClientSocket::OnReadAsync,
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