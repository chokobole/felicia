#include "felicia/core/channel/tcp_client_channel.h"

#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

TCPClientChannel::TCPClientChannel() = default;
TCPClientChannel::~TCPClientChannel() = default;

void TCPClientChannel::Connect(const ::net::IPEndPoint& ip_endpoint,
                               StatusCallback callback) {
  DCHECK(connect_callback_.is_null());
  DCHECK(!callback.is_null());
  auto client_socket = std::make_unique<::net::TCPSocket>(nullptr);
  int rv = client_socket->Open(ip_endpoint.GetFamily());
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  client_socket->SetDefaultOptionsForClient();

  rv = client_socket->Connect(
      ip_endpoint,
      ::base::BindOnce(&TCPClientChannel::OnConnect, ::base::Unretained(this)));
  if (rv != ::net::OK && rv != ::net::ERR_IO_PENDING) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  connect_callback_ = std::move(callback);
  socket_ = std::move(client_socket);

  if (rv == ::net::OK) {
    std::move(connect_callback_).Run(Status::OK());
  }
}

void TCPClientChannel::OnConnect(int result) {
  if (result == ::net::OK) {
    std::move(connect_callback_).Run(Status::OK());
  } else {
    std::move(connect_callback_)
        .Run(errors::NetworkError(::net::ErrorToString(result)));
  }
}

void TCPClientChannel::Write(::net::IOBufferWithSize* buffer,
                             StatusCallback callback) {
  DCHECK(!callback.is_null());
  write_callback_ = std::move(callback);
  if (!socket_->IsConnected()) {
    OnWrite(::net::ERR_SOCKET_NOT_CONNECTED);
    return;
  }
  int rv = socket_->Write(
      buffer, buffer->size(),
      ::base::BindOnce(&TCPClientChannel::OnWrite, ::base::Unretained(this)),
      ::net::DefineNetworkTrafficAnnotation("tcp_client_channel",
                                            "Send Message"));
  if (rv != ::net::ERR_IO_PENDING) {
    OnWrite(rv);
  }
}

void TCPClientChannel::Read(::net::IOBufferWithSize* buffer,
                            StatusCallback callback) {
  DCHECK(!callback.is_null());
  read_callback_ = std::move(callback);
  if (!socket_->IsConnected()) {
    OnRead(::net::ERR_SOCKET_NOT_CONNECTED);
    return;
  }
  int rv = socket_->Read(
      buffer, buffer->size(),
      ::base::BindOnce(&TCPClientChannel::OnRead, ::base::Unretained(this)));
  if (rv != ::net::ERR_IO_PENDING) {
    OnRead(rv);
  }
}

}  // namespace felicia