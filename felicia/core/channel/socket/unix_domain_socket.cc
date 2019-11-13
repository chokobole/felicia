// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/socket/unix_domain_socket.h"

namespace felicia {

UnixDomainSocket::UnixDomainSocket() = default;

UnixDomainSocket::UnixDomainSocket(std::unique_ptr<net::SocketPosix> socket)
    : socket_(std::move(socket)) {}

UnixDomainSocket::~UnixDomainSocket() = default;

bool UnixDomainSocket::IsUnixDomainSocket() const { return true; }

int UnixDomainSocket::Write(net::IOBuffer* buf, int buf_len,
                            net::CompletionOnceCallback callback) {
  DCHECK(socket_);
  return socket_->Write(
      buf, buf_len, std::move(callback),
      net::DefineNetworkTrafficAnnotation("UnixDomainSocket", "Write"));
}

int UnixDomainSocket::Read(net::IOBuffer* buf, int buf_len,
                           net::CompletionOnceCallback callback) {
  DCHECK(socket_);
  return socket_->Read(buf, buf_len, std::move(callback));
}

void UnixDomainSocket::Close() {
  DCHECK(socket_);
  socket_->Close();
}

UnixDomainClientSocket* UnixDomainSocket::ToUnixDomainClientSocket() {
  DCHECK(IsClient());
  return reinterpret_cast<UnixDomainClientSocket*>(this);
}

UnixDomainServerSocket* UnixDomainSocket::ToUnixDomainServerSocket() {
  DCHECK(IsServer());
  return reinterpret_cast<UnixDomainServerSocket*>(this);
}

}  // namespace felicia