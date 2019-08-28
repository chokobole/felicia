// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/socket/ssl_client_socket_impl.cc

#if !defined(FEL_NO_SSL)

#include "felicia/core/channel/socket/ssl_client_socket.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/memory/singleton.h"
#include "third_party/chromium/net/ssl/ssl_key_logger.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

namespace {

// This constant can be any non-negative/non-zero value (eg: it does not
// overlap with any value of the net::Error range, including net::OK).
const int kSSLClientSocketNoPendingResult = 1;
// This constant can be any non-negative/non-zero value (eg: it does not
// overlap with any value of the net::Error range, including net::OK).
const int kCertVerifyPending = 1;

// Default size of the internal BoringSSL buffers.
const int kDefaultOpenSSLBufferSize = 17 * 1024;

}  // namespace

class SSLClientSocket::SSLContext {
 public:
  static SSLContext* GetInstance() {
    return base::Singleton<SSLContext,
                           base::LeakySingletonTraits<SSLContext>>::get();
  }
  SSL_CTX* ssl_ctx() { return ssl_ctx_.get(); }

  SSLClientSocket* GetClientSocketFromSSL(const SSL* ssl) {
    DCHECK(ssl);
    SSLClientSocket* socket = static_cast<SSLClientSocket*>(
        SSL_get_ex_data(ssl, ssl_socket_data_index_));
    DCHECK(socket);
    return socket;
  }

  bool SetClientSocketForSSL(SSL* ssl, SSLClientSocket* socket) {
    return SSL_set_ex_data(ssl, ssl_socket_data_index_, socket) != 0;
  }

  void SetSSLKeyLogger(std::unique_ptr<net::SSLKeyLogger> logger) {
    DCHECK(!ssl_key_logger_);
    ssl_key_logger_ = std::move(logger);
    SSL_CTX_set_keylog_callback(ssl_ctx_.get(), KeyLogCallback);
  }

  static const SSL_PRIVATE_KEY_METHOD kPrivateKeyMethod;

 private:
  friend struct base::DefaultSingletonTraits<SSLContext>;

  SSLContext() {
    crypto::EnsureOpenSSLInit();
    ssl_socket_data_index_ = SSL_get_ex_new_index(0, 0, 0, 0, 0);
    DCHECK_NE(ssl_socket_data_index_, -1);
    ssl_ctx_.reset(SSL_CTX_new(TLS_client_method()));

    // Verifies the server certificate even on resumed sessions.
    SSL_CTX_set_reverify_on_resume(ssl_ctx_.get(), 1);

    SSL_CTX_set_session_cache_mode(ssl_ctx_.get(), SSL_SESS_CACHE_CLIENT);
    SSL_CTX_set_timeout(ssl_ctx_.get(), 1 * 60 * 60 /* one hour */);

    SSL_CTX_set_grease_enabled(ssl_ctx_.get(), 1);

    SSL_CTX_set_info_callback(ssl_ctx_.get(), InfoCallback);
    SSL_CTX_set_msg_callback(ssl_ctx_.get(), MessageCallback);

    static const int kCurves[] = {NID_CECPQ2, NID_X25519, NID_X9_62_prime256v1,
                                  NID_secp384r1};
    SSL_CTX_set1_curves(ssl_ctx_.get(), kCurves, base::size(kCurves));
  }

  static void KeyLogCallback(const SSL* ssl, const char* line) {
    GetInstance()->ssl_key_logger_->WriteLine(line);
  }

  static void InfoCallback(const SSL* ssl, int type, int value) {
    SSLClientSocket* socket = GetInstance()->GetClientSocketFromSSL(ssl);
    socket->InfoCallback(type, value);
  }

  static void MessageCallback(int is_write, int version, int content_type,
                              const void* buf, size_t len, SSL* ssl,
                              void* arg) {
    SSLClientSocket* socket = GetInstance()->GetClientSocketFromSSL(ssl);
    socket->MessageCallback(is_write, content_type, buf, len);
  }

  // This is the index used with SSL_get_ex_data to retrieve the owner
  // SSLClientSocket object from an SSL instance.
  int ssl_socket_data_index_;

  ::bssl::UniquePtr<SSL_CTX> ssl_ctx_;

  std::unique_ptr<net::SSLKeyLogger> ssl_key_logger_;
};

SSLClientSocket::SSLClientSocket(std::unique_ptr<StreamSocket> stream_socket)
    : SSLSocket(std::move(stream_socket)),
      pending_read_error_(kSSLClientSocketNoPendingResult),
      pending_read_ssl_error_(SSL_ERROR_NONE),
      completed_connect_(false),
      next_handshake_state_(STATE_NONE),
      disconnected_(false),
      weak_factory_(this) {}

SSLClientSocket::~SSLClientSocket() { Close(); }

void SSLClientSocket::Connect(StatusOnceCallback callback) {
  // Although StreamSocket does allow calling Connect() after Disconnect(),
  // this has never worked for layered sockets. CHECK to detect any consumers
  // reconnecting an SSL socket.
  //
  // TODO(davidben,mmenke): Remove this API feature. See
  // https://crbug.com/499289.
  CHECK(!disconnected_);
  int rv = Init();

  // Set SSL to client mode. Handshake happens in the loop below.
  SSL_set_connect_state(ssl_.get());

  next_handshake_state_ = STATE_HANDSHAKE;
  rv = DoHandshakeLoop(net::OK);
  rv = rv > net::OK ? net::OK : rv;
  if (rv != net::OK && rv != net::ERR_IO_PENDING) {
    std::move(callback).Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  connect_callback_ = std::move(callback);

  if (rv == net::OK) {
    std::move(connect_callback_).Run(Status::OK());
  }
}

bool SSLClientSocket::IsClient() const { return true; }

bool SSLClientSocket::IsConnected() const {
  // If the handshake has not yet completed or the socket has been explicitly
  // disconnected.
  if (!completed_connect_ || disconnected_) return false;
  // If an asynchronous operation is still pending.
  if (user_read_buf_.get() || user_write_buf_.get()) return true;

  return stream_socket_ && stream_socket_->IsConnected();
}

int SSLClientSocket::Write(net::IOBuffer* buf, int buf_len,
                           net::CompletionOnceCallback callback) {
  user_write_buf_ = buf;
  user_write_buf_len_ = buf_len;

  int rv = DoPayloadWrite();
  if (rv == net::ERR_IO_PENDING) {
    user_write_callback_ = std::move(callback);
  } else {
    user_write_buf_ = NULL;
    user_write_buf_len_ = 0;
  }

  return rv;
}

int SSLClientSocket::Read(net::IOBuffer* buf, int buf_len,
                          net::CompletionOnceCallback callback) {
  user_read_buf_ = buf;
  user_read_buf_len_ = buf_len;

  int rv = DoPayloadRead();
  if (rv == net::ERR_IO_PENDING) {
    user_read_callback_ = std::move(callback);
  } else {
    user_read_buf_ = NULL;
    user_read_buf_len_ = 0;
  }

  return rv;
}

void SSLClientSocket::Close() {
  disconnected_ = true;

  // Shut down anything that may call us back.
  weak_factory_.InvalidateWeakPtrs();
  transport_adapter_.reset();

  // Release user callbacks.
  connect_callback_.Reset();
  user_read_callback_.Reset();
  user_write_callback_.Reset();
  user_read_buf_ = NULL;
  user_read_buf_len_ = 0;
  user_write_buf_ = NULL;
  user_write_buf_len_ = 0;

  stream_socket_->Close();
}

void SSLClientSocket::WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                                 StatusOnceCallback callback) {
  int result = Write(buffer.get(), size,
                     base::BindOnce(&SSLClientSocket::OnWriteCheckingReset,
                                    base::Unretained(this)));
  if (result == net::ERR_IO_PENDING) {
    write_callback_ = std::move(callback);
  } else {
    Socket::CallbackWithStatus(std::move(callback), result);
  }
}

void SSLClientSocket::ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer,
                                int size, StatusOnceCallback callback) {
  int result = Read(buffer.get(), size,
                    base::BindOnce(&SSLClientSocket::OnReadCheckingClosed,
                                   base::Unretained(this)));
  if (result == net::ERR_IO_PENDING) {
    read_callback_ = std::move(callback);
  } else {
    Socket::CallbackWithStatus(std::move(callback), result);
  }
}

void SSLClientSocket::OnReadReady() {
  // During a renegotiation, either Read or Write calls may be blocked on a
  // transport read.
  RetryAllOperations();
}

void SSLClientSocket::OnWriteReady() {
  // During a renegotiation, either Read or Write calls may be blocked on a
  // transport read.
  RetryAllOperations();
}

int SSLClientSocket::Init() {
  DCHECK(!ssl_);

  SSLContext* context = SSLContext::GetInstance();
  crypto::OpenSSLErrStackTracer err_tracer(FROM_HERE);

  ssl_.reset(SSL_new(context->ssl_ctx()));
  if (!ssl_ || !context->SetClientSocketForSSL(ssl_.get(), this))
    return net::ERR_UNEXPECTED;

  transport_adapter_.reset(
      new SocketBIOAdapter(stream_socket_.get(), kDefaultOpenSSLBufferSize,
                           kDefaultOpenSSLBufferSize, this));
  BIO* transport_bio = transport_adapter_->bio();

  BIO_up_ref(transport_bio);  // SSL_set0_rbio takes ownership.
  SSL_set0_rbio(ssl_.get(), transport_bio);

  BIO_up_ref(transport_bio);  // SSL_set0_wbio takes ownership.
  SSL_set0_wbio(ssl_.get(), transport_bio);

  if (!SSL_set_min_proto_version(ssl_.get(), TLS1_2_VERSION) ||
      !SSL_set_max_proto_version(ssl_.get(), TLS1_3_VERSION)) {
    return net::ERR_UNEXPECTED;
  }

  // In felicia, in most cases, RTT is not an issue.
  SSL_set_early_data_enabled(ssl_.get(), false);

  SSL_set_ignore_tls13_downgrade(ssl_.get(), 1);

  // OpenSSL defaults some options to on, others to off. To avoid ambiguity,
  // set everything we care about to an absolute value.
  net::SslSetClearMask options;
  options.ConfigureFlag(SSL_OP_NO_COMPRESSION, true);

  // TODO(joth): Set this conditionally, see http://crbug.com/55410
  options.ConfigureFlag(SSL_OP_LEGACY_SERVER_CONNECT, true);

  SSL_set_options(ssl_.get(), options.set_mask);
  SSL_clear_options(ssl_.get(), options.clear_mask);

  // Same as above, this time for the SSL mode.
  net::SslSetClearMask mode;

  mode.ConfigureFlag(SSL_MODE_RELEASE_BUFFERS, true);
  mode.ConfigureFlag(SSL_MODE_CBC_RECORD_SPLITTING, true);

  // For most cases in felicia, RTT is not that critical.
  // So explicitly turned it off.
  mode.ConfigureFlag(SSL_MODE_ENABLE_FALSE_START, false);

  SSL_set_mode(ssl_.get(), mode.set_mask);
  SSL_clear_mode(ssl_.get(), mode.clear_mask);

  SSL_enable_signed_cert_timestamps(ssl_.get());
  SSL_enable_ocsp_stapling(ssl_.get());

  SSL_set_shed_handshake_config(ssl_.get(), 1);
  return net::OK;
}

void SSLClientSocket::DoReadCallback(int rv) {
  // Since Run may result in Read being called, clear |user_read_callback_|
  // up front.
  user_read_buf_ = nullptr;
  user_read_buf_len_ = 0;
  std::move(user_read_callback_).Run(rv);
}

void SSLClientSocket::DoWriteCallback(int rv) {
  // Since Run may result in Write being called, clear |user_write_callback_|
  // up front.
  user_write_buf_ = NULL;
  user_write_buf_len_ = 0;
  std::move(user_write_callback_).Run(rv);
}

int SSLClientSocket::DoHandshake() {
  crypto::OpenSSLErrStackTracer err_tracer(FROM_HERE);

  int rv = SSL_do_handshake(ssl_.get());
  int net_error = net::OK;
  if (rv <= 0) {
    int ssl_error = SSL_get_error(ssl_.get(), rv);
    if (ssl_error == SSL_ERROR_WANT_X509_LOOKUP) {
      return net::ERR_SSL_CLIENT_AUTH_CERT_NEEDED;
    }
    if (ssl_error == SSL_ERROR_WANT_PRIVATE_KEY_OPERATION) {
      next_handshake_state_ = STATE_HANDSHAKE;
      return net::ERR_IO_PENDING;
    }
    if (ssl_error == SSL_ERROR_WANT_CERTIFICATE_VERIFY) {
      next_handshake_state_ = STATE_HANDSHAKE;
      return net::ERR_IO_PENDING;
    }

    net::OpenSSLErrorInfo error_info;
    net_error = MapLastOpenSSLError(ssl_error, err_tracer, &error_info);
    if (net_error == net::ERR_IO_PENDING) {
      // If not done, stay in this state
      next_handshake_state_ = STATE_HANDSHAKE;
      return net::ERR_IO_PENDING;
    }

    LOG(ERROR) << "handshake failed; returned " << rv << ", SSL error code "
               << ssl_error << ", net_error " << net_error;
  }

  next_handshake_state_ = STATE_HANDSHAKE_COMPLETE;
  return net_error;
}

int SSLClientSocket::DoHandshakeComplete(int result) {
  if (result < 0) return result;

  completed_connect_ = true;
  next_handshake_state_ = STATE_NONE;
  return net::OK;
}

void SSLClientSocket::DoConnectCallback(int rv) {
  rv = rv > net::OK ? net::OK : rv;
  Socket::OnConnect(rv);
}

void SSLClientSocket::OnHandshakeIOComplete(int result) {
  int rv = DoHandshakeLoop(result);
  if (rv != net::ERR_IO_PENDING) {
    if (!connect_callback_.is_null()) DoConnectCallback(rv);
  }
}

int SSLClientSocket::DoHandshakeLoop(int last_io_result) {
  int rv = last_io_result;
  do {
    // Default to STATE_NONE for next state.
    // (This is a quirk carried over from the windows
    // implementation.  It makes reading the logs a bit harder.)
    // State handlers can and often do call GotoState just
    // to stay in the current state.
    State state = next_handshake_state_;
    next_handshake_state_ = STATE_NONE;
    switch (state) {
      case STATE_HANDSHAKE:
        rv = DoHandshake();
        break;
      case STATE_HANDSHAKE_COMPLETE:
        rv = DoHandshakeComplete(rv);
        break;
      case STATE_NONE:
      default:
        rv = net::ERR_UNEXPECTED;
        NOTREACHED() << "unexpected state" << state;
        break;
    }
  } while (rv != net::ERR_IO_PENDING && next_handshake_state_ != STATE_NONE);
  return rv;
}

int SSLClientSocket::DoPayloadRead() {
  crypto::OpenSSLErrStackTracer err_tracer(FROM_HERE);

  DCHECK_LT(0, user_read_buf_len_);
  DCHECK(user_read_buf_);

  int rv;
  if (pending_read_error_ != kSSLClientSocketNoPendingResult) {
    rv = pending_read_error_;
    pending_read_error_ = kSSLClientSocketNoPendingResult;
    pending_read_ssl_error_ = SSL_ERROR_NONE;
    pending_read_error_info_ = net::OpenSSLErrorInfo();
    return rv;
  }

  int total_bytes_read = 0;
  int ssl_ret;
  do {
    ssl_ret = SSL_read(ssl_.get(), user_read_buf_->data() + total_bytes_read,
                       user_read_buf_len_ - total_bytes_read);
    if (ssl_ret > 0) total_bytes_read += ssl_ret;
    // Continue processing records as long as there is more data available
    // synchronously.
  } while (total_bytes_read < user_read_buf_len_ && ssl_ret > 0 &&
           transport_adapter_->HasPendingReadData());

  // Although only the final SSL_read call may have failed, the failure needs to
  // processed immediately, while the information still available in OpenSSL's
  // error queue.
  if (ssl_ret <= 0) {
    pending_read_ssl_error_ = SSL_get_error(ssl_.get(), ssl_ret);
    if (pending_read_ssl_error_ == SSL_ERROR_ZERO_RETURN) {
      pending_read_error_ = 0;
    } else if (pending_read_ssl_error_ == SSL_ERROR_WANT_X509_LOOKUP) {
      pending_read_error_ = net::ERR_SSL_CLIENT_AUTH_CERT_NEEDED;
    } else if (pending_read_ssl_error_ ==
               SSL_ERROR_WANT_PRIVATE_KEY_OPERATION) {
      pending_read_error_ = net::ERR_IO_PENDING;
    } else {
      pending_read_error_ = MapLastOpenSSLError(
          pending_read_ssl_error_, err_tracer, &pending_read_error_info_);
    }

    // Many servers do not reliably send a close_notify alert when shutting down
    // a connection, and instead terminate the TCP connection. This is reported
    // as ERR_CONNECTION_CLOSED. Because of this, map the unclean shutdown to a
    // graceful EOF, instead of treating it as an error as it should be.
    if (pending_read_error_ == net::ERR_CONNECTION_CLOSED)
      pending_read_error_ = 0;
  }

  if (total_bytes_read > 0) {
    // Return any bytes read to the caller. The error will be deferred to the
    // next call of DoPayloadRead.
    rv = total_bytes_read;

    // Do not treat insufficient data as an error to return in the next call to
    // DoPayloadRead() - instead, let the call fall through to check SSL_read()
    // again. The transport may have data available by then.
    if (pending_read_error_ == net::ERR_IO_PENDING)
      pending_read_error_ = kSSLClientSocketNoPendingResult;
  } else {
    // No bytes were returned. Return the pending read error immediately.
    DCHECK_NE(kSSLClientSocketNoPendingResult, pending_read_error_);
    rv = pending_read_error_;
    pending_read_error_ = kSSLClientSocketNoPendingResult;
  }

  if (rv < 0 && rv != net::ERR_IO_PENDING) {
    pending_read_ssl_error_ = SSL_ERROR_NONE;
    pending_read_error_info_ = net::OpenSSLErrorInfo();
  }
  return rv;
}

int SSLClientSocket::DoPayloadWrite() {
  crypto::OpenSSLErrStackTracer err_tracer(FROM_HERE);
  int rv = SSL_write(ssl_.get(), user_write_buf_->data(), user_write_buf_len_);

  if (rv >= 0) {
    if (first_post_handshake_write_ && SSL_is_init_finished(ssl_.get())) {
      if (SSL_version(ssl_.get()) == TLS1_3_VERSION) {
        const int ok = SSL_key_update(ssl_.get(), SSL_KEY_UPDATE_REQUESTED);
        DCHECK(ok);
      }
      first_post_handshake_write_ = false;
    }
    return rv;
  }

  int ssl_error = SSL_get_error(ssl_.get(), rv);
  if (ssl_error == SSL_ERROR_WANT_PRIVATE_KEY_OPERATION)
    return net::ERR_IO_PENDING;
  net::OpenSSLErrorInfo error_info;
  return MapLastOpenSSLError(ssl_error, err_tracer, &error_info);
}

void SSLClientSocket::RetryAllOperations() {
  // SSL_do_handshake, SSL_read, and SSL_write may all be retried when blocked,
  // so retry all operations for simplicity. (Otherwise, SSL_get_error for each
  // operation may be remembered to retry only the blocked ones.)

  // Performing these callbacks may cause |this| to be deleted. If this
  // happens, the other callbacks should not be invoked. Guard against this by
  // holding a WeakPtr to |this| and ensuring it's still valid.
  base::WeakPtr<SSLClientSocket> guard(weak_factory_.GetWeakPtr());
  if (next_handshake_state_ == STATE_HANDSHAKE) {
    // In handshake phase. The parameter to OnHandshakeIOComplete is unused.
    OnHandshakeIOComplete(net::OK);
  }

  if (!guard.get()) return;

  int rv_read = net::ERR_IO_PENDING;
  int rv_write = net::ERR_IO_PENDING;
  if (user_read_buf_) {
    rv_read = DoPayloadRead();
  } else if (!user_read_callback_.is_null()) {
    // ReadIfReady() is called by the user. Skip DoPayloadRead() and just let
    // the user know that read can be retried.
    rv_read = net::OK;
  }

  if (user_write_buf_) rv_write = DoPayloadWrite();

  if (rv_read != net::ERR_IO_PENDING) DoReadCallback(rv_read);

  if (!guard.get()) return;

  if (rv_write != net::ERR_IO_PENDING) DoWriteCallback(rv_write);
}

void SSLClientSocket::InfoCallback(int type, int value) {
  LOG(INFO) << "InfoCallback: type: " << type << ", value: " << value;
}

void SSLClientSocket::MessageCallback(int is_write, int content_type,
                                      const void* buf, size_t len) {
  switch (content_type) {
    case SSL3_RT_ALERT:
      LOG(INFO) << "MessageCallback: SSL3_RT_ALERT";
      break;
    case SSL3_RT_HANDSHAKE:
      LOG(INFO) << "MessageCallback: SSL3_RT_HANDSHAKE";
      break;
    default:
      return;
  }
}

int SSLClientSocket::MapLastOpenSSLError(
    int ssl_error, const crypto::OpenSSLErrStackTracer& tracer,
    net::OpenSSLErrorInfo* info) {
  int net_error = MapOpenSSLErrorWithDetails(ssl_error, tracer, info);

  if (ssl_error == SSL_ERROR_SSL &&
      ERR_GET_LIB(info->error_code) == ERR_LIB_SSL) {
    // Per spec, access_denied is only for client-certificate-based access
    // control, but some buggy firewalls use it when blocking a page. To avoid a
    // confusing error, map it to a generic protocol error if no
    // CertificateRequest was sent. See https://crbug.com/630883.
    if (ERR_GET_REASON(info->error_code) == SSL_R_TLSV1_ALERT_ACCESS_DENIED) {
      net_error = net::ERR_SSL_PROTOCOL_ERROR;
    }

    // This error is specific to the client, so map it here.
    if (ERR_GET_REASON(info->error_code) ==
        SSL_R_NO_COMMON_SIGNATURE_ALGORITHMS) {
      net_error = net::ERR_SSL_CLIENT_AUTH_NO_COMMON_ALGORITHMS;
    }
  }

  return net_error;
}

}  // namespace felicia

#endif  // !defined(FEL_NO_SSL)