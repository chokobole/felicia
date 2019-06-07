#include "felicia/core/channel/socket/tcp_server_socket.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/net/net_util.h"

namespace felicia {

TCPServerSocket::TCPServerSocket() = default;
TCPServerSocket::~TCPServerSocket() = default;

const std::vector<std::unique_ptr<::net::TCPSocket>>&
TCPServerSocket::accepted_sockets() const {
  return accepted_sockets_;
}

bool TCPServerSocket::IsServer() const { return true; }

bool TCPServerSocket::IsConnected() const {
  for (auto& accepted_socket : accepted_sockets_) {
    if (accepted_socket->IsConnected()) return true;
  }
  return false;
}

StatusOr<ChannelDef> TCPServerSocket::Listen() {
  auto server_socket = std::make_unique<::net::TCPSocket>(nullptr);

  int rv = server_socket->Open(::net::ADDRESS_FAMILY_IPV4);
  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  uint16_t port = net::PickRandomPort(true);
  ::net::IPAddress address(0, 0, 0, 0);
  ::net::IPEndPoint server_endpoint(address, port);
  rv = server_socket->Bind(server_endpoint);
  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  rv = server_socket->SetDefaultOptionsForServer();
  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  rv = server_socket->Listen(5);
  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  socket_ = std::move(server_socket);

  return ToChannelDef(
      ::net::IPEndPoint(net::HostIPAddress(net::HOST_IP_ONLY_ALLOW_IPV4), port),
      ChannelDef::TCP);
}

void TCPServerSocket::AcceptLoop(AcceptCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(accept_callback_.is_null() &&
         accept_once_intercept_callback_.is_null());
  accept_callback_ = callback;
  DoAcceptLoop();
}

void TCPServerSocket::AcceptOnceIntercept(
    AcceptOnceInterceptCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(accept_callback_.is_null() &&
         accept_once_intercept_callback_.is_null());
  accept_once_intercept_callback_ = std::move(callback);
  DoAccept();
}

void TCPServerSocket::AddSocket(std::unique_ptr<::net::TCPSocket> socket) {
  accepted_sockets_.push_back(std::move(socket));
}

void TCPServerSocket::Write(scoped_refptr<::net::IOBuffer> buffer, int size,
                            StatusOnceCallback callback) {
  DCHECK_EQ(0, to_write_count_);
  DCHECK_EQ(0, written_count_);
  DCHECK(write_callback_.is_null());
  DCHECK(!callback.is_null());
  DCHECK(size > 0);

  EraseClosedSockets();

  if (accepted_sockets_.size() == 0) {
    std::move(callback).Run(errors::NetworkError(
        ::net::ErrorToString(::net::ERR_SOCKET_NOT_CONNECTED)));
    return;
  }

  to_write_count_ = accepted_sockets_.size();
  write_callback_ = std::move(callback);
  auto it = accepted_sockets_.begin();
  while (it != accepted_sockets_.end()) {
    scoped_refptr<::net::DrainableIOBuffer> write_buffer =
        ::base::MakeRefCounted<::net::DrainableIOBuffer>(
            buffer, static_cast<size_t>(size));
    while (write_buffer->BytesRemaining() > 0) {
      int rv =
          (*it)->Write(write_buffer.get(), write_buffer->BytesRemaining(),
                       ::base::BindOnce(&TCPServerSocket::OnWrite,
                                        ::base::Unretained(this), (*it).get()),
                       ::net::DefineNetworkTrafficAnnotation(
                           "tcp_server_socket", "Send Message"));

      if (rv == ::net::ERR_IO_PENDING) break;

      if (rv >= 0) {
        write_buffer->DidConsume(rv);
      }

      if (write_buffer->BytesRemaining() == 0 || rv <= 0) {
        OnWrite((*it).get(), rv);
        break;
      }
    }

    it++;
  }
}

void TCPServerSocket::Read(scoped_refptr<::net::GrowableIOBuffer> buffer,
                           int size, StatusOnceCallback callback) {
  NOTREACHED() << "You read data from ServerSocket, if you need, please use "
                  "TCPServerSokcet::AcceptOnceIntercept.";
}

int TCPServerSocket::DoAccept() {
  int result = socket_->Accept(
      &accepted_socket_, &accepted_endpoint_,
      ::base::BindOnce(&TCPServerSocket::OnAccept, ::base::Unretained(this)));
  if (result != ::net::ERR_IO_PENDING) HandleAccpetResult(result);
  return result;
}

void TCPServerSocket::DoAcceptLoop() {
  int result = ::net::OK;
  while (result == ::net::OK) {
    result = DoAccept();
  }
}

void TCPServerSocket::HandleAccpetResult(int result) {
  DCHECK_NE(result, ::net::ERR_IO_PENDING);

  if (result < 0) {
    LOG(ERROR) << "Error when accepting a connection: "
               << ::net::ErrorToString(result);
    return;
  }

  if (accept_once_intercept_callback_) {
    std::move(accept_once_intercept_callback_).Run(std::move(accepted_socket_));
  } else {
    accepted_sockets_.push_back(std::move(accepted_socket_));
    if (accept_callback_) accept_callback_.Run(Status::OK());
  }
}

void TCPServerSocket::OnAccept(int result) {
  HandleAccpetResult(result);
  if (accept_callback_) DoAcceptLoop();
}

void TCPServerSocket::OnWrite(::net::TCPSocket* socket, int result) {
  if (result == ::net::ERR_CONNECTION_RESET) {
    socket->Close();
    has_closed_sockets_ = true;
  }

  written_count_++;
  if (result < 0) {
    LOG(ERROR) << "TCPServerSocket::OnWrite: " << ::net::ErrorToString(result);
    write_result_ = result;
  }
  if (to_write_count_ == written_count_) {
    to_write_count_ = 0;
    written_count_ = 0;
    int write_result = write_result_;
    write_result_ = 0;
    CallbackWithStatus(std::move(write_callback_), write_result);
  }
}

void TCPServerSocket::EraseClosedSockets() {
  if (has_closed_sockets_) {
    auto it = accepted_sockets_.begin();
    while (it != accepted_sockets_.end()) {
      if (!(*it)->IsConnected()) {
        it = accepted_sockets_.erase(it);
        continue;
      }
      it++;
    }
    has_closed_sockets_ = false;
  }
}

}  // namespace felicia