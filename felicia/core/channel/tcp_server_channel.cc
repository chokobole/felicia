#include "felicia/core/channel/tcp_server_channel.h"

#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/strings/strcat.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/platform/net_util.h"

namespace felicia {

TCPServerChannel::TCPServerChannel() = default;
TCPServerChannel::~TCPServerChannel() = default;

void TCPServerChannel::Listen(const NodeInfo& node_info,
                              StatusOrIPEndPointCallback callback) {
  DCHECK(callback);
  auto server_socket = std::make_unique<::net::TCPSocket>(nullptr);

  ::net::IPAddress ip;
  if (!ip.AssignFromIPLiteral(node_info.ip_endpoint().ip())) {
    std::move(callback).Run(errors::InvalidArgument(::base::StrCat(
        {"IP Address is not valid ", node_info.ip_endpoint().ip()})));
    return;
  }

  int rv;
  if (ip.IsIPv4()) {
    rv = server_socket->Open(::net::AddressFamily::ADDRESS_FAMILY_IPV4);
  } else {
    rv = server_socket->Open(::net::AddressFamily::ADDRESS_FAMILY_IPV6);
  }
  if (rv != ::net::OK) {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(rv)));
    return;
  }

  uint16_t port = node_info.ip_endpoint().port();
  if (port == 0) {
    port = net::PickRandomPort(true);
  }
  ::net::IPAddress address(0, 0, 0, 0);
  ::net::IPEndPoint endpoint(address, port);
  rv = server_socket->Bind(endpoint);
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
  std::move(callback).Run(::net::IPEndPoint(ip, port));
  DoAcceptLoop();
}

void TCPServerChannel::Write(::net::IOBuffer* buf, size_t buf_len,
                             StatusCallback callback) {
  DCHECK_EQ(0, to_write_count_);
  DCHECK_EQ(0, written_count_);
  DCHECK(callback);
  to_write_count_ = accepted_sockets_.size();
  write_callback_ = callback;
  for (auto& socket : accepted_sockets_) {
    int rv = socket->Write(
        buf, buf_len,
        ::base::BindOnce(&TCPServerChannel::OnWrite, ::base::Unretained(this)),
        ::net::DefineNetworkTrafficAnnotation("tcp_server_channel",
                                              "Send Message"));
    if (rv != ::net::ERR_IO_PENDING) {
      OnWrite(rv);
    }
  };
}

void TCPServerChannel::Read(::net::IOBuffer* buf, size_t buf_len,
                            StatusCallback callback) {
  DCHECK(callback);
  DCHECK_EQ(1, accepted_sockets_.size());
  read_callback_ = callback;
  int rv = accepted_sockets_[0]->Read(
      buf, buf_len,
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
  accept_callback_(Status::OK());
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
    to_write_count_ = 0;
    written_count_ = 0;
    if (write_result_ >= 0) {
      write_callback_(Status::OK());
    } else {
      write_callback_(
          errors::NetworkError(::net::ErrorToString(write_result_)));
    }
  }
}

}  // namespace felicia