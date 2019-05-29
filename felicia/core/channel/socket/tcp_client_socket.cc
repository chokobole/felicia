#include "felicia/core/channel/socket/tcp_client_socket.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

TCPClientSocket::TCPClientSocket() = default;
TCPClientSocket::~TCPClientSocket() = default;

void TCPClientSocket::set_socket(std::unique_ptr<::net::TCPSocket> socket) {
  socket_ = std::move(socket);
}

bool TCPClientSocket::IsClient() const { return true; }

bool TCPClientSocket::IsConnected() const {
  return socket_ && socket_->IsConnected();
}

void TCPClientSocket::Connect(const ::net::IPEndPoint& ip_endpoint,
                              StatusOnceCallback callback) {
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
      ::base::BindOnce(&TCPClientSocket::OnConnect, ::base::Unretained(this)));
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

void TCPClientSocket::Write(scoped_refptr<::net::IOBuffer> buffer, int size,
                            StatusOnceCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  write_callback_ = std::move(callback);
  scoped_refptr<::net::DrainableIOBuffer> write_buffer =
      ::base::MakeRefCounted<::net::DrainableIOBuffer>(
          buffer, static_cast<size_t>(size));
  while (write_buffer->BytesRemaining() > 0) {
    int rv = socket_->Write(
        write_buffer.get(), write_buffer->BytesRemaining(),
        ::base::BindOnce(&TCPClientSocket::OnWrite, ::base::Unretained(this)),
        ::net::DefineNetworkTrafficAnnotation("tcp_client_socket",
                                              "Send Message"));

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

void TCPClientSocket::Read(scoped_refptr<::net::GrowableIOBuffer> buffer,
                           int size, StatusOnceCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(size > 0);
  read_callback_ = std::move(callback);
  int to_read = size;
  while (to_read > 0) {
    int rv = socket_->Read(
        buffer.get(), to_read,
        ::base::BindOnce(&TCPClientSocket::OnRead, ::base::Unretained(this)));

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

void TCPClientSocket::OnConnect(int result) {
  CallbackWithStatus(std::move(connect_callback_), result);
}

void TCPClientSocket::OnWrite(int result) {
  if (result == ::net::ERR_CONNECTION_RESET) {
    socket_.reset();
  }
  CallbackWithStatus(std::move(write_callback_), result);
}

void TCPClientSocket::OnRead(int result) {
  if (result == 0) {
    result = ::net::ERR_CONNECTION_CLOSED;
    socket_.reset();
  }
  CallbackWithStatus(std::move(read_callback_), result);
}

}  // namespace felicia