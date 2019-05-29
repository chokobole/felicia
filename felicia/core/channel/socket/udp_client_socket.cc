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

void UDPClientSocket::Write(scoped_refptr<::net::IOBuffer> buffer, int size,
                            StatusOnceCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  write_callback_ = std::move(callback);
  scoped_refptr<::net::DrainableIOBuffer> write_buffer =
      ::base::MakeRefCounted<::net::DrainableIOBuffer>(
          buffer, static_cast<size_t>(size));
  while (write_buffer->BytesRemaining() > 0) {
    int rv = socket_->SendTo(
        write_buffer.get(), write_buffer->BytesRemaining(),
        multicast_ip_endpoint_,
        ::base::BindOnce(&UDPClientSocket::OnWrite, ::base::Unretained(this)));

    if (rv == ::net::ERR_IO_PENDING) break;

    if (rv > 0) {
      write_buffer->DidConsume(rv);
    }

    if (write_buffer->BytesRemaining() == 0 || rv <= 0) {
      OnWrite(rv);
      break;
    }
  }
}

void UDPClientSocket::Read(scoped_refptr<::net::GrowableIOBuffer> buffer,
                           int size, StatusOnceCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  read_callback_ = std::move(callback);
  int to_read = size;
  while (to_read > 0) {
    int rv = socket_->Read(
        buffer.get(), to_read,
        ::base::BindOnce(&UDPClientSocket::OnRead, ::base::Unretained(this)));

    if (rv == ::net::ERR_IO_PENDING) break;

    if (rv > 0) {
      buffer->set_offset(buffer->offset() + rv);
      to_read -= rv;
    }

    if (to_read == 0 || rv <= 0) {
      OnRead(rv);
      break;
    }
  }
}

}  // namespace felicia