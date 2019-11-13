// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELIICA_CORE_CHANNEL_SOCKET_SSL_SOCKET_H_
#define FELIICA_CORE_CHANNEL_SOCKET_SSL_SOCKET_H_

#include "felicia/core/channel/socket/stream_socket.h"

namespace felicia {

class SSLClientSocket;
class SSLServerSocket;

class SSLSocket : public StreamSocket {
 public:
  SSLSocket();
  explicit SSLSocket(std::unique_ptr<StreamSocket> stream_socket);
  ~SSLSocket() override;

  bool IsSSLSocket() const override;

  SSLClientSocket* ToSSLClientSocket();
  SSLServerSocket* ToSSLServerSocket();

 protected:
  void OnWriteCheckingReset(int result);
  void OnReadCheckingClosed(int result);

  std::unique_ptr<StreamSocket> stream_socket_;

  DISALLOW_COPY_AND_ASSIGN(SSLSocket);
};

}  // namespace felicia

#endif  // FELIICA_CORE_CHANNEL_SOCKET_SSL_SOCKET_H_
