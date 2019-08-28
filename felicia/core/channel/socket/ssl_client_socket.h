// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/socket/ssl_client_socket_impl.h

#if !defined(FEL_NO_SSL)

#ifndef FELICIA_CORE_CHANNEL_SOCKET_SSL_CLIENT_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_SSL_CLIENT_SOCKET_H_

#include "openssl/base.h"
#include "openssl/ssl.h"

#include "third_party/chromium/crypto/openssl_util.h"
#include "third_party/chromium/net/ssl/openssl_ssl_util.h"

#include "felicia/core/channel/socket/socket_bio_adapter.h"
#include "felicia/core/channel/socket/ssl_socket.h"
#include "felicia/core/channel/socket/stream_socket.h"

namespace felicia {

class SSLClientSocket : public SSLSocket, public SocketBIOAdapter::Delegate {
 public:
  SSLClientSocket(std::unique_ptr<StreamSocket> stream_socket);
  ~SSLClientSocket();

  void Connect(StatusOnceCallback callback);

  // Socket methods
  bool IsClient() const override;
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
  class SSLContext;
  friend class SSLContext;

  int Init();
  void DoReadCallback(int result);
  void DoWriteCallback(int result);

  int DoHandshake();
  int DoHandshakeComplete(int result);
  void DoConnectCallback(int result);

  void OnVerifyComplete(int result);
  void OnHandshakeIOComplete(int result);

  int DoHandshakeLoop(int last_io_result);
  int DoPayloadRead();
  int DoPayloadWrite();

  // Called when an asynchronous event completes which may have blocked the
  // pending Connect, Read or Write calls, if any. Retries all state machines
  // and, if complete, runs the respective callbacks.
  void RetryAllOperations();

  // Called from the BoringSSL info callback. (See |SSL_CTX_set_info_callback|.)
  void InfoCallback(int type, int value);

  // Called whenever BoringSSL processes a protocol message.
  void MessageCallback(int is_write, int content_type, const void* buf,
                       size_t len);

  // Returns the net error corresponding to the most recent OpenSSL
  // error. ssl_error is the output of SSL_get_error.
  int MapLastOpenSSLError(int ssl_error,
                          const crypto::OpenSSLErrStackTracer& tracer,
                          net::OpenSSLErrorInfo* info);

  net::CompletionOnceCallback user_read_callback_;
  net::CompletionOnceCallback user_write_callback_;

  // Used by Read function.
  scoped_refptr<net::IOBuffer> user_read_buf_;
  int user_read_buf_len_;

  // Used by Write function.
  scoped_refptr<net::IOBuffer> user_write_buf_;
  int user_write_buf_len_;
  bool first_post_handshake_write_ = true;

  // Used by DoPayloadRead() when attempting to fill the caller's buffer with
  // as much data as possible without blocking.
  // If DoPayloadRead() encounters an error after having read some data, stores
  // the result to return on the *next* call to DoPayloadRead().  A value > 0
  // indicates there is no pending result, otherwise 0 indicates EOF and < 0
  // indicates an error.
  int pending_read_error_;

  // If there is a pending read result, the OpenSSL result code (output of
  // SSL_get_error) associated with it.
  int pending_read_ssl_error_;

  // If there is a pending read result, the OpenSSLErrorInfo associated with it.
  net::OpenSSLErrorInfo pending_read_error_info_;

  bool completed_connect_;

  // OpenSSL stuff
  bssl::UniquePtr<SSL> ssl_;

  std::unique_ptr<SocketBIOAdapter> transport_adapter_;

  enum State {
    STATE_NONE,
    STATE_HANDSHAKE,
    STATE_HANDSHAKE_COMPLETE,
  };
  State next_handshake_state_;

  // True if the socket has been disconnected.
  bool disconnected_;

  base::WeakPtrFactory<SSLClientSocket> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(SSLClientSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_SSL_CLIENT_SOCKET_H_

#endif  // !defined(FEL_NO_SSL)