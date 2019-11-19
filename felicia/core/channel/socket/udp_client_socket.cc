// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/socket/udp_client_socket.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

UDPClientSocket::UDPClientSocket() = default;
UDPClientSocket::~UDPClientSocket() = default;

void UDPClientSocket::Connect(const net::AddressList& addrlist,
                              StatusOnceCallback callback) {
  DCHECK(!socket_);
  DCHECK(!callback.is_null());
  DCHECK(connect_callback_.is_null());

  addrlist_idx_ = 0;
  addrlist_ = addrlist;
  connect_callback_ = std::move(callback);

  DoConnect();
}

bool UDPClientSocket::IsClient() const { return true; }

void UDPClientSocket::WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                                 StatusOnceCallback callback) {
  WriteRepeating(
      buffer, size, std::move(callback),
      base::BindRepeating(&UDPClientSocket::OnWrite, base::Unretained(this)));
}

void UDPClientSocket::ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer,
                                int size, StatusOnceCallback callback) {
  ReadRepeating(
      buffer, size, std::move(callback),
      base::BindRepeating(&UDPClientSocket::OnRead, base::Unretained(this)));
}

void UDPClientSocket::DoConnect() {
  if (static_cast<size_t>(addrlist_idx_) >= addrlist_.size()) {
    std::move(connect_callback_)
        .Run(errors::NetworkError(
            net::ErrorToString(net::ERR_CONNECTION_FAILED)));
    return;
  }
  net::IPEndPoint ip_endpoint = addrlist_[addrlist_idx_];

  auto client_socket = std::make_unique<net::UDPSocket>(
      net::DatagramSocket::BindType::DEFAULT_BIND);
  int rv = client_socket->Open(ip_endpoint.GetFamily());
  if (rv != net::OK) {
    std::move(connect_callback_)
        .Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->SetMulticastLoopbackMode(true);
  if (rv != net::OK) {
    std::move(connect_callback_)
        .Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->AllowAddressSharingForMulticast();
  if (rv != net::OK) {
    std::move(connect_callback_)
        .Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  net::IPAddress address(0, 0, 0, 0);
  net::IPEndPoint endpoint(address, ip_endpoint.port());
  rv = client_socket->Bind(endpoint);
  if (rv != net::OK) {
    std::move(connect_callback_)
        .Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  rv = client_socket->JoinGroup(ip_endpoint.address());
  if (rv != net::OK) {
    std::move(connect_callback_)
        .Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  socket_ = std::move(client_socket);
  multicast_ip_endpoint_ = ip_endpoint;
  std::move(connect_callback_).Run(Status::OK());
}

void UDPClientSocket::OnConnect(int result) {
  if (result == net::OK) {
    std::move(connect_callback_).Run(Status::OK());
  } else {
    ++addrlist_idx_;
    DoConnect();
  }
}

}  // namespace felicia