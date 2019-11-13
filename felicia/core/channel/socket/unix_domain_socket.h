// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_SOCKET_UNIX_DOMAIN_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_UNIX_DOMAIN_SOCKET_H_

#include "third_party/chromium/net/socket/socket_posix.h"

#include "felicia/core/channel/socket/stream_socket.h"

namespace felicia {

class UnixDomainClientSocket;
class UnixDomainServerSocket;

class UnixDomainSocket : public StreamSocket {
 public:
  UnixDomainSocket();
  explicit UnixDomainSocket(std::unique_ptr<net::SocketPosix> socket);
  ~UnixDomainSocket();

  // Socket mehtods
  bool IsUnixDomainSocket() const override;
  int Write(net::IOBuffer* buf, int buf_len,
            net::CompletionOnceCallback callback) override;
  int Read(net::IOBuffer* buf, int buf_len,
           net::CompletionOnceCallback callback) override;
  void Close() override;

  UnixDomainClientSocket* ToUnixDomainClientSocket();
  UnixDomainServerSocket* ToUnixDomainServerSocket();

 protected:
  std::unique_ptr<net::SocketPosix> socket_;

  DISALLOW_COPY_AND_ASSIGN(UnixDomainSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_UNIX_DOMAIN_SOCKET_H_