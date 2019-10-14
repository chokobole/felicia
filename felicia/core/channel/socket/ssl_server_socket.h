// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/socket/ssl_server_socket_impl.h

#if !defined(FEL_NO_SSL)

#ifndef FELICIA_CORE_CHANNEL_SOCKET_SSL_SERVER_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_SSL_SERVER_SOCKET_H_

#include "openssl/ssl.h"

#include "third_party/chromium/base/timer/timer.h"
#include "third_party/chromium/crypto/openssl_util.h"
#include "third_party/chromium/net/ssl/openssl_ssl_util.h"

#include "felicia/core/channel/socket/socket_bio_adapter.h"
#include "felicia/core/channel/socket/ssl_socket.h"
#include "felicia/core/channel/socket/stream_socket.h"

namespace felicia {

class SSLServerContext;

class SSLServerSocket : public SSLSocket, public SocketBIOAdapter::Delegate {
 public:
  SSLServerSocket(SSLServerContext* context,
                  std::unique_ptr<StreamSocket> stream_socket);
  ~SSLServerSocket();

  void Handshake(StatusOnceCallback callback);

  // Socket methods
  bool IsServer() const override;
  bool IsConnected() const override;
  int Write(net::IOBuffer* buf, int buf_len,
            net::CompletionOnceCallback callback) override;
  int Read(net::IOBuffer* buf, int buf_len,
           net::CompletionOnceCallback callback) override;
  void Close() override;

  // ChannelImpl methods
  void WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                  StatusOnceCallback callback) override;
  void ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer, int size,
                 StatusOnceCallback callback) override;

  // SocketBIOAdapter::Delegate methods
  void OnReadReady() override;
  void OnWriteReady() override;

 private:
  enum State {
    STATE_NONE,
    STATE_HANDSHAKE,
  };

  void OnHandshakeIOComplete(int result);

  int DoPayloadRead();
  int DoPayloadWrite();

  int DoHandshakeLoop(int last_io_result);
  int DoHandshake();
  void DoHandshakeCallback(int result);
  void DoReadCallback(int result);
  void DoWriteCallback(int result);

  void HandshakeTimeout();

  int Init();

  SSLServerContext* context_;
  std::unique_ptr<SocketBIOAdapter> transport_adapter_;

  net::CompletionOnceCallback user_read_callback_;
  net::CompletionOnceCallback user_write_callback_;

  // Used by Read function.
  scoped_refptr<net::IOBuffer> user_read_buf_;
  int user_read_buf_len_;

  // Used by Write function.
  scoped_refptr<net::IOBuffer> user_write_buf_;
  int user_write_buf_len_;

  // OpenSSL stuff
  bssl::UniquePtr<SSL> ssl_;

  // Whether we received any data in early data.
  bool early_data_received_;

  State next_handshake_state_;
  bool completed_handshake_;

  // Timer for the handshake.
  base::OneShotTimer handshake_timer_;

  base::TimeDelta handshake_timeout_;

  DISALLOW_COPY_AND_ASSIGN(SSLServerSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_SSL_SERVER_SOCKET_H_

#endif  // !defined(FEL_NO_SSL)