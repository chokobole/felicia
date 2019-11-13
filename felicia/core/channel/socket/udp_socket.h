// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_SOCKET_UDP_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_UDP_SOCKET_H_

#include "third_party/chromium/net/socket/udp_socket.h"

#include "felicia/core/channel/socket/datagram_socket.h"

namespace felicia {

class UDPClientSocket;
class UDPServerSocket;

class UDPSocket : public DatagramSocket {
 public:
  UDPSocket();
  ~UDPSocket() override;

  // Socket methods
  bool IsUDPSocket() const override;
  int Write(net::IOBuffer* buf, int buf_len,
            net::CompletionOnceCallback callback) override;
  int Read(net::IOBuffer* buf, int buf_len,
           net::CompletionOnceCallback callback) override;
  void Close() override;

  // DatagramSocket methods
  bool IsConnected() const override;

  UDPClientSocket* ToUDPClientSocket();
  UDPServerSocket* ToUDPServerSocket();

 protected:
  std::unique_ptr<net::UDPSocket> socket_;
  net::IPEndPoint multicast_ip_endpoint_;

  DISALLOW_COPY_AND_ASSIGN(UDPSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_UDP_SOCKET_H_