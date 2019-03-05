#include "felicia/core/channel/tcp_server_channel.h"

#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/strings/strcat.h"
#include "third_party/chromium/base/threading/thread_task_runner_handle.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/platform/net_util.h"

namespace felicia {

TCPServerChannel::TCPServerChannel() = default;
TCPServerChannel::~TCPServerChannel() = default;

bool TCPServerChannel::IsConnected() const {
  auto it = accepted_sockets_.begin();
  while (it != accepted_sockets_.end()) {
    if ((*it)->IsConnected()) {
      return true;
    }
    it++;
  }

  return false;
}

void TCPServerChannel::Listen(StatusOrChannelSourceCallback callback) {
  DCHECK(callback);
  auto server_socket = std::make_unique<::net::TCPSocket>(nullptr);

  int rv = server_socket->Open(::net::ADDRESS_FAMILY_IPV4);
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  uint16_t port = net::PickRandomPort(true);
  ::net::IPAddress address(0, 0, 0, 0);
  ::net::IPEndPoint server_endpoint(address, port);
  rv = server_socket->Bind(server_endpoint);
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  rv = server_socket->SetDefaultOptionsForServer();
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  rv = server_socket->Listen(5);
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  socket_ = std::move(server_socket);

  ChannelDef channel_def;
  channel_def.set_type(ChannelDef_Type_TCP);
  ChannelSource channel_source = ToChannelSource(
      ::net::IPEndPoint(net::HostIPAddress(net::HOST_IP_ONLY_ALLOW_IPV4), port),
      channel_def);

  std::move(callback).Run(channel_source);
  DoAcceptLoop();
}

void TCPServerChannel::Write(::net::IOBufferWithSize* buffer,
                             StatusCallback callback) {
  DCHECK_EQ(0, to_write_count_);
  DCHECK_EQ(0, written_count_);
  DCHECK(write_callback_.is_null());
  DCHECK(!callback.is_null());
  write_callback_ = std::move(callback);
  auto it = accepted_sockets_.begin();

  while (it != accepted_sockets_.end() && !(*it)->IsConnected()) {
    it = accepted_sockets_.erase(it);
  }

  if (it == accepted_sockets_.end()) {
    // Overrided OnWrite doesn't call |write_callback_| because
    // |to_write_count_| and |writte_count_| not matched.
    ChannelBase::OnWrite(::net::ERR_SOCKET_NOT_CONNECTED);
    return;
  }

  to_write_count_ = accepted_sockets_.size();
  it = accepted_sockets_.begin();

  while (it != accepted_sockets_.end()) {
    int rv = (*it)->Write(
        buffer, buffer->size(),
        ::base::BindOnce(&TCPServerChannel::OnWrite, ::base::Unretained(this)),
        ::net::DefineNetworkTrafficAnnotation("tcp_server_channel",
                                              "Send Message"));
    if (rv != ::net::ERR_IO_PENDING) {
      OnWrite(rv);
    }

    it++;
  }

  if (!write_callback_.is_null()) {
    timeout_.Reset(::base::BindOnce(&TCPServerChannel::OnWriteTimeout,
                                    ::base::Unretained(this)));
    ::base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
        FROM_HERE, timeout_.callback(), ::base::TimeDelta::FromSeconds(5));
  }
}

void TCPServerChannel::Read(::net::IOBufferWithSize* buffer,
                            StatusCallback callback) {
  DCHECK(read_callback_.is_null());
  DCHECK(!callback.is_null());
  read_callback_ = std::move(callback);
  auto it = accepted_sockets_.begin();

  while (it != accepted_sockets_.end() && !(*it)->IsConnected()) {
    it = accepted_sockets_.erase(it);
  }

  if (it == accepted_sockets_.end()) {
    OnRead(::net::ERR_SOCKET_NOT_CONNECTED);
    return;
  }

  int rv = (*it)->Read(
      buffer, buffer->size(),
      ::base::BindOnce(&TCPServerChannel::OnRead, ::base::Unretained(this)));
  if (rv != ::net::ERR_IO_PENDING) {
    OnRead(rv);
  }
}

void TCPServerChannel::DoAcceptLoop() {
  int result = ::net::OK;
  while (result == ::net::OK) {
    result = socket_->Accept(&accepted_socket_, &accepted_endpoint_,
                             ::base::BindOnce(&TCPServerChannel::OnAccept,
                                              ::base::Unretained(this)));
    if (result != ::net::ERR_IO_PENDING) HandleAccpetResult(result);
  }
}

void TCPServerChannel::HandleAccpetResult(int result) {
  DCHECK_NE(result, ::net::ERR_IO_PENDING);

  if (result < 0) {
    LOG(ERROR) << "Error when accepting a connection: "
               << ::net::ErrorToString(result);
    return;
  }

  accepted_sockets_.push_back(std::move(accepted_socket_));
  accept_callback_.Run(Status::OK());
}

void TCPServerChannel::OnAccept(int result) {
  HandleAccpetResult(result);
  if (result == ::net::OK) {
    DoAcceptLoop();
  }
}

void TCPServerChannel::OnWrite(int result) {
  written_count_++;
  if (result < 0) {
    LOG(ERROR) << "TCPServerChannel::OnWrite: " << ::net::ErrorToString(result);
    write_result_ = result;
  }
  if (to_write_count_ == written_count_) {
    timeout_.Cancel();
    to_write_count_ = 0;
    written_count_ = 0;
    if (write_result_ >= 0) {
      std::move(write_callback_).Run(Status::OK());
    } else {
      std::move(write_callback_)
          .Run(errors::NetworkError(::net::ErrorToString(write_result_)));
    }
  }
}

void TCPServerChannel::OnWriteTimeout() {
  auto it = accepted_sockets_.begin();
  while (it != accepted_sockets_.end()) {
    if (!(*it)->IsConnected()) {
      it = accepted_sockets_.erase(it);
      continue;
    }
    it++;
  }

  to_write_count_ = 0;
  written_count_ = 0;

  std::move(write_callback_)
      .Run(errors::NetworkError(::net::ErrorToString(::net::ERR_FAILED)));
}

}  // namespace felicia