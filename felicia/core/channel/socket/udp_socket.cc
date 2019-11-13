// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/socket/udp_socket.h"

namespace felicia {

UDPSocket::UDPSocket() = default;
UDPSocket::~UDPSocket() = default;

bool UDPSocket::IsUDPSocket() const { return true; }

int UDPSocket::Write(net::IOBuffer* buf, int buf_len,
                     net::CompletionOnceCallback callback) {
  DCHECK(socket_);
  return socket_->SendTo(buf, buf_len, multicast_ip_endpoint_,
                         std::move(callback));
}

int UDPSocket::Read(net::IOBuffer* buf, int buf_len,
                    net::CompletionOnceCallback callback) {
  DCHECK(socket_);
  return socket_->Read(buf, buf_len, std::move(callback));
}

void UDPSocket::Close() {
  DCHECK(socket_);
  socket_->Close();
}

bool UDPSocket::IsConnected() const {
  return socket_ && socket_->is_connected();
}

UDPClientSocket* UDPSocket::ToUDPClientSocket() {
  DCHECK(IsClient());
  return reinterpret_cast<UDPClientSocket*>(this);
}

UDPServerSocket* UDPSocket::ToUDPServerSocket() {
  DCHECK(IsServer());
  return reinterpret_cast<UDPServerSocket*>(this);
}

}  // namespace felicia