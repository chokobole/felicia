// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_

#include "third_party/chromium/net/base/address_list.h"

#include "felicia/core/channel/socket/tcp_socket.h"

namespace felicia {

class TCPClientSocket : public TCPSocket {
 public:
  TCPClientSocket();
  explicit TCPClientSocket(std::unique_ptr<net::TCPSocket> socket);
  ~TCPClientSocket();

  void Connect(const net::AddressList& addrlist, StatusOnceCallback callback);

  // Socket methods
  bool IsClient() const override;
  bool IsConnected() const override;

  // ChannelImpl methods
  void WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                  StatusOnceCallback callback) override;
  void ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer, int size,
                 StatusOnceCallback callback) override;

 private:
  void DoConnect();
  void OnConnect(int result);

  void OnWriteCheckingReset(int result);
  void OnReadCheckingClosed(int result);

  net::AddressList addrlist_;
  int addrlist_idx_;

  DISALLOW_COPY_AND_ASSIGN(TCPClientSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_