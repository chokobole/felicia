// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_SOCKET_UDP_CLIENT_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_UDP_CLIENT_SOCKET_H_

#include "felicia/core/channel/socket/udp_socket.h"

namespace felicia {

class UDPClientSocket : public UDPSocket {
 public:
  UDPClientSocket();
  ~UDPClientSocket();

  void Connect(const net::IPEndPoint& ip_endpoint, StatusOnceCallback callback);

  // Socket methods
  bool IsClient() const override;

  // ChannelImpl methods
  void WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                  StatusOnceCallback callback) override;
  void ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer, int size,
                 StatusOnceCallback callback) override;

  DISALLOW_COPY_AND_ASSIGN(UDPClientSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_UDP_CLIENT_SOCKET_H_