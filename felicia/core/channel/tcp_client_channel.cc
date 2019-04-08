#include "felicia/core/channel/tcp_client_channel.h"

#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

TCPClientChannel::TCPClientChannel() = default;
TCPClientChannel::~TCPClientChannel() = default;

bool TCPClientChannel::IsClient() const { return true; }

bool TCPClientChannel::IsConnected() const {
  return socket_ && socket_->IsConnected();
}

void TCPClientChannel::Connect(const ::net::IPEndPoint& ip_endpoint,
                               StatusCallback callback) {
  DCHECK(!socket_);
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

void TCPClientChannel::Write(::net::IOBuffer* buffer, int size,
                             StatusCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  write_callback_ = std::move(callback);
  int to_write = size;
  int written = 0;
  while (to_write > 0) {
    int rv = socket_->Write(
        buffer + written, to_write,
        ::base::BindOnce(&TCPClientChannel::OnWrite, ::base::Unretained(this)),
        ::net::DefineNetworkTrafficAnnotation("tcp_client_channel",
                                              "Send Message"));

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

void TCPClientChannel::Read(::net::IOBuffer* buffer, int size,
                            StatusCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  read_callback_ = std::move(callback);
  int to_read = size;
  int read = 0;
  while (to_read > 0) {
    int rv = socket_->Read(
        buffer + read, to_read,
        ::base::BindOnce(&TCPClientChannel::OnRead, ::base::Unretained(this)));

    if (rv == ::net::ERR_IO_PENDING) break;

    if (rv >= 0) {
      to_read -= rv;
      read += rv;
    }

    if (to_read == 0 || rv <= 0) {
      OnRead(rv);
      break;
    }
  }
}

void TCPClientChannel::OnConnect(int result) {
  CallbackWithStatus(std::move(connect_callback_), result);
}

void TCPClientChannel::OnWrite(int result) {
  if (result == ::net::ERR_CONNECTION_RESET) {
    socket_.reset();
  }
  CallbackWithStatus(std::move(write_callback_), result);
}

void TCPClientChannel::OnRead(int result) {
  if (result == 0) {
    result = ::net::ERR_CONNECTION_CLOSED;
    socket_.reset();
  }
  CallbackWithStatus(std::move(read_callback_), result);
}

}  // namespace felicia