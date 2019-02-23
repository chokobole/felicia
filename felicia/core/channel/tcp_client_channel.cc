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
  DCHECK(!connect_callback_);
  DCHECK(callback);
  auto client_socket = std::make_unique<::net::TCPSocket>(nullptr);
  int rv = client_socket->Open(ip_endpoint.GetFamily());
  if (rv != ::net::OK) {
    callback(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  client_socket->SetDefaultOptionsForClient();

  rv = client_socket->Connect(
      ip_endpoint,
      ::base::BindOnce(&TCPClientChannel::OnConnect, ::base::Unretained(this)));
  if (rv != ::net::OK && rv != ::net::ERR_IO_PENDING) {
    callback(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  connect_callback_ = callback;
  socket_ = std::move(client_socket);

  if (rv == ::net::OK) {
    callback(Status::OK());
  }
}

void TCPClientChannel::OnConnect(int result) {
  LOG(INFO) << "TCPClientChannel::OnConnect()";
  if (result == ::net::OK) {
    connect_callback_(Status::OK());
  } else {
    connect_callback_(errors::NetworkError(::net::ErrorToString(result)));
  }
}

void TCPClientChannel::Write(::net::IOBuffer* buf, size_t buf_len,
                             StatusCallback callback) {
  DCHECK(callback);
  write_callback_ = callback;
  int rv = socket_->Write(
      buf, buf_len,
      ::base::BindOnce(&TCPClientChannel::OnWrite, ::base::Unretained(this)),
      ::net::DefineNetworkTrafficAnnotation("tcp_client_channel",
                                            "Send Message"));
  if (rv != ::net::ERR_IO_PENDING) {
    OnWrite(rv);
  }
}

void TCPClientChannel::Read(::net::IOBuffer* buf, size_t buf_len,
                            StatusCallback callback) {
  DCHECK(callback);
  read_callback_ = callback;
  int rv = socket_->Read(
      buf, buf_len,
      ::base::BindOnce(&TCPClientChannel::OnRead, ::base::Unretained(this)));
  if (rv != ::net::ERR_IO_PENDING) {
    OnRead(rv);
  }
}

}  // namespace felicia