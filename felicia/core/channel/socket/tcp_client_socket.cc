// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/socket/tcp_client_socket.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

TCPClientSocket::TCPClientSocket() = default;

TCPClientSocket::TCPClientSocket(std::unique_ptr<net::TCPSocket> socket)
    : TCPSocket(std::move(socket)) {}

TCPClientSocket::~TCPClientSocket() = default;

void TCPClientSocket::Connect(const net::AddressList& addrlist,
                              StatusOnceCallback callback) {
  DCHECK(!socket_);
  DCHECK(connect_callback_.is_null());
  DCHECK(!callback.is_null());

  addrlist_idx_ = 0;
  addrlist_ = addrlist;
  connect_callback_ = std::move(callback);

  DoConnect();
}

bool TCPClientSocket::IsClient() const { return true; }

bool TCPClientSocket::IsConnected() const {
  return socket_ && socket_->IsConnected();
}

void TCPClientSocket::WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                                 StatusOnceCallback callback) {
  WriteRepeating(
      buffer, size, std::move(callback),
      base::BindRepeating(&TCPClientSocket::OnWrite, base::Unretained(this)));
}

void TCPClientSocket::ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer,
                                int size, StatusOnceCallback callback) {
  ReadRepeating(
      buffer, size, std::move(callback),
      base::BindRepeating(&TCPClientSocket::OnRead, base::Unretained(this)));
}

void TCPClientSocket::DoConnect() {
  if (static_cast<size_t>(addrlist_idx_) >= addrlist_.size()) {
    std::move(connect_callback_)
        .Run(errors::NetworkError(
            net::ErrorToString(net::ERR_CONNECTION_FAILED)));
    return;
  }
  net::IPEndPoint ip_endpoint = addrlist_[addrlist_idx_];

  auto client_socket = std::make_unique<net::TCPSocket>(nullptr);
  int rv = client_socket->Open(ip_endpoint.GetFamily());
  if (rv != net::OK) {
    std::move(connect_callback_)
        .Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  client_socket->SetDefaultOptionsForClient();

  rv = client_socket->Connect(
      ip_endpoint,
      base::BindOnce(&TCPClientSocket::OnConnect, base::Unretained(this)));
  if (rv != net::OK && rv != net::ERR_IO_PENDING) {
    std::move(connect_callback_)
        .Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  socket_ = std::move(client_socket);

  if (rv == net::OK) {
    std::move(connect_callback_).Run(Status::OK());
  }
}

void TCPClientSocket::OnConnect(int result) {
  if (result == net::OK) {
    std::move(connect_callback_).Run(Status::OK());
  } else {
    ++addrlist_idx_;
    DoConnect();
  }
}

void TCPClientSocket::OnWriteCheckingReset(int result) {
  if (result == net::ERR_CONNECTION_RESET) {
    socket_.reset();
  }
  Socket::OnWrite(result);
}

void TCPClientSocket::OnReadCheckingClosed(int result) {
  if (result == 0) {
    result = net::ERR_CONNECTION_CLOSED;
    socket_.reset();
  }
  Socket::OnRead(result);
}

}  // namespace felicia