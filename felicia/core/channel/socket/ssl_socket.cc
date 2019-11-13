// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/socket/ssl_socket.h"

namespace felicia {

SSLSocket::SSLSocket() = default;

SSLSocket::SSLSocket(std::unique_ptr<StreamSocket> stream_socket)
    : stream_socket_(std::move(stream_socket)) {}

SSLSocket::~SSLSocket() = default;

bool SSLSocket::IsSSLSocket() const { return true; }

SSLClientSocket* SSLSocket::ToSSLClientSocket() {
  DCHECK(IsClient());
  return reinterpret_cast<SSLClientSocket*>(this);
}

SSLServerSocket* SSLSocket::ToSSLServerSocket() {
  DCHECK(IsServer());
  return reinterpret_cast<SSLServerSocket*>(this);
}

void SSLSocket::OnWriteCheckingReset(int result) {
  if (result == net::ERR_CONNECTION_RESET) {
    stream_socket_.reset();
  }
  Socket::OnWrite(result);
}

void SSLSocket::OnReadCheckingClosed(int result) {
  if (result == 0) {
    result = net::ERR_CONNECTION_CLOSED;
    stream_socket_.reset();
  }
  Socket::OnRead(result);
}

}  // namespace felicia