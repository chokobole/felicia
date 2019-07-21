// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/socket/ssl_server_socket_impl.cc

#if !defined(FEL_NO_SSL)

#include "felicia/core/channel/socket/ssl_server_socket.h"

#include "third_party/chromium/base/lazy_instance.h"

#include "felicia/core/channel/socket/ssl_server_context.h"
#include "felicia/core/lib/error/errors.h"

#define GotoState(s) next_handshake_state_ = s

namespace felicia {

namespace {

class SocketDataIndex {
 public:
  static SocketDataIndex* GetInstance();
  SocketDataIndex() {
    ssl_socket_data_index_ = SSL_get_ex_new_index(0, 0, 0, 0, 0);
  }

  // This is the index used with SSL_get_ex_data to retrieve the owner
  // SSLServerSocketImpl object from an SSL instance.
  int ssl_socket_data_index_;
};

base::LazyInstance<SocketDataIndex>::Leaky g_ssl_socket_data_index_ =
    LAZY_INSTANCE_INITIALIZER;

// static
SocketDataIndex* SocketDataIndex::GetInstance() {
  return g_ssl_socket_data_index_.Pointer();
}

}  // namespace

SSLServerSocket::SSLServerSocket(SSLServerContext* context,
                                 std::unique_ptr<StreamSocket> stream_socket) HEAD
    : context_(context),
      stream_socket_(std::move(stream_socket)),
      user_read_buf_len_(0),
      user_write_buf_len_(0),
      early_data_received_(false),
      next_handshake_state_(STATE_NONE),
      completed_handshake_(false) {
  ssl_.reset(SSL_new(context_->ssl_ctx_.get()));
  SSL_set_app_data(ssl_.get(), this);
  SSL_set_shed_handshake_config(ssl_.get(), 1);
}

SSLServerSocket::~SSLServerSocket() {
  if (ssl_) {
    // Calling SSL_shutdown prevents the session from being marked as
    // unresumable.
    SSL_shutdown(ssl_.get());
    ssl_.reset();
  }
}

// static
const SSL_PRIVATE_KEY_METHOD SSLServerSocket::kPrivateKeyMethod = {
    &SSLServerSocket::PrivateKeySignCallback,
    &SSLServerSocket::PrivateKeyDecryptCallback,
    &SSLServerSocket::PrivateKeyCompleteCallback,
};

// static
ssl_private_key_result_t SSLServerSocket::PrivateKeySignCallback(
    SSL* ssl, uint8_t* out, size_t* out_len, size_t max_out, uint16_t algorithm,
    const uint8_t* in, size_t in_len) {
  DCHECK(ssl);
  SSLServerSocket* socket = static_cast<SSLServerSocket*>(SSL_get_ex_data(
      ssl, SocketDataIndex::GetInstance()->ssl_socket_data_index_));
  DCHECK(socket);
  return socket->PrivateKeySignCallback(out, out_len, max_out, algorithm, in,
                                        in_len);
}

// static
ssl_private_key_result_t SSLServerSocket::PrivateKeyDecryptCallback(
    SSL* ssl, uint8_t* out, size_t* out_len, size_t max_out, const uint8_t* in,
    size_t in_len) {
  // Decrypt is not supported.
  return ssl_private_key_failure;
}

// static
ssl_private_key_result_t SSLServerSocket::PrivateKeyCompleteCallback(
    SSL* ssl, uint8_t* out, size_t* out_len, size_t max_out) {
  DCHECK(ssl);
  SSLServerSocket* socket = static_cast<SSLServerSocket*>(SSL_get_ex_data(
      ssl, SocketDataIndex::GetInstance()->ssl_socket_data_index_));
  DCHECK(socket);
  return socket->PrivateKeyCompleteCallback(out, out_len, max_out);
}

ssl_private_key_result_t SSLServerSocket::PrivateKeySignCallback(
    uint8_t* out, size_t* out_len, size_t max_out, uint16_t algorithm,
    const uint8_t* in, size_t in_len) {
  DCHECK(context_);
  // DCHECK(context_->private_key_);
  signature_result_ = net::ERR_IO_PENDING;
  // context_->private_key_->Sign(
  //     algorithm, base::make_span(in, in_len),
  //     base::BindRepeating(
  //         &SSLServerSocket::OnPrivateKeyComplete,
  //         weak_factory_.GetWeakPtr()));
  NOTIMPLEMENTED();
  return ssl_private_key_retry;
}

ssl_private_key_result_t SSLServerSocket::PrivateKeyCompleteCallback(
    uint8_t* out, size_t* out_len, size_t max_out) {
  if (signature_result_ == net::ERR_IO_PENDING) return ssl_private_key_retry;
  if (signature_result_ != net::OK) {
    net::OpenSSLPutNetError(FROM_HERE, signature_result_);
    return ssl_private_key_failure;
  }
  if (signature_.size() > max_out) {
    net::OpenSSLPutNetError(FROM_HERE,
                            net::ERR_SSL_CLIENT_AUTH_SIGNATURE_FAILED);
    return ssl_private_key_failure;
  }
  memcpy(out, signature_.data(), signature_.size());
  *out_len = signature_.size();
  signature_.clear();
  return ssl_private_key_success;
}

void SSLServerSocket::OnPrivateKeyComplete(
    net::Error error, const std::vector<uint8_t>& signature) {
  DCHECK_EQ(net::ERR_IO_PENDING, signature_result_);
  DCHECK(signature_.empty());

  signature_result_ = error;
  if (signature_result_ == net::OK) signature_ = signature;
  DoHandshakeLoop(net::ERR_IO_PENDING);
}

void SSLServerSocket::Handshake(StatusOnceCallback callback) {
  // Set up new ssl object.
  int rv = Init();
  if (rv != net::OK) {
    std::move(callback).Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  }

  // Set SSL to server mode. Handshake happens in the loop below.
  SSL_set_accept_state(ssl_.get());

  GotoState(STATE_HANDSHAKE);
  rv = DoHandshakeLoop(net::OK);
  rv = rv > net::OK ? net::OK : rv;
  if (rv != net::OK && rv != net::ERR_IO_PENDING) {
    std::move(callback).Run(errors::NetworkError(net::ErrorToString(rv)));
    return;
  } else if (rv == net::OK) {
    std::move(callback).Run(Status::OK());
    return;
  }

  connect_callback_ = std::move(callback);
}

bool SSLServerSocket::IsClient() const { return true; }

bool SSLServerSocket::IsConnected() const {
  return completed_handshake_ && stream_socket_ &&
         stream_socket_->IsConnected();
}

int SSLServerSocket::Write(net::IOBuffer* buf, int buf_len,
                           net::CompletionOnceCallback callback) {
  DCHECK(user_write_callback_.is_null());
  DCHECK(!user_write_buf_);
  DCHECK(!callback.is_null());

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

int SSLServerSocket::Read(net::IOBuffer* buf, int buf_len,
                          net::CompletionOnceCallback callback) {
  DCHECK(user_read_callback_.is_null());
  DCHECK(connect_callback_.is_null());
  DCHECK(!user_read_buf_);
  DCHECK(!callback.is_null());

  user_read_buf_ = buf;
  user_read_buf_len_ = buf_len;

  DCHECK(completed_handshake_);

  int rv = DoPayloadRead();

  if (rv == net::ERR_IO_PENDING) {
    user_read_callback_ = std::move(callback);
  } else {
    user_read_buf_ = NULL;
    user_read_buf_len_ = 0;
  }

  return rv;
}

void SSLServerSocket::Close() { stream_socket_->Close(); }

void SSLServerSocket::Write(scoped_refptr<net::IOBuffer> buffer, int size,
                            StatusOnceCallback callback) {
  int result = Write(
      buffer.get(), size,
      ::base::BindOnce(&SSLServerSocket::OnWrite, ::base::Unretained(this)));
  if (result == net::ERR_IO_PENDING) {
    write_callback_ = std::move(callback);
  } else {
    Socket::CallbackWithStatus(std::move(callback), result);
  }
}

void SSLServerSocket::Read(scoped_refptr<net::GrowableIOBuffer> buffer,
                           int size, StatusOnceCallback callback) {
  int result = Read(
      buffer.get(), size,
      ::base::BindOnce(&SSLServerSocket::OnRead, ::base::Unretained(this)));
  if (result == net::ERR_IO_PENDING) {
    read_callback_ = std::move(callback);
  } else {
    Socket::CallbackWithStatus(std::move(callback), result);
  }
}

void SSLServerSocket::OnReadReady() {
  if (next_handshake_state_ == STATE_HANDSHAKE) {
    // In handshake phase. The parameter to OnHandshakeIOComplete is unused.
    OnHandshakeIOComplete(net::OK);
    return;
  }

  // BoringSSL does not support renegotiation as a server, so the only other
  // operation blocked on Read is DoPayloadRead.
  if (!user_read_buf_) return;

  int rv = DoPayloadRead();
  if (rv != net::ERR_IO_PENDING) DoReadCallback(rv);
}

void SSLServerSocket::OnWriteReady() {
  if (next_handshake_state_ == STATE_HANDSHAKE) {
    // In handshake phase. The parameter to OnHandshakeIOComplete is unused.
    OnHandshakeIOComplete(net::OK);
    return;
  }

  // BoringSSL does not support renegotiation as a server, so the only other
  // operation blocked on Read is DoPayloadWrite.
  if (!user_write_buf_) return;

  int rv = DoPayloadWrite();
  if (rv != net::ERR_IO_PENDING) DoWriteCallback(rv);
}

void SSLServerSocket::OnHandshakeIOComplete(int result) {
  int rv = DoHandshakeLoop(result);
  if (rv == net::ERR_IO_PENDING) return;

  if (!connect_callback_.is_null()) DoHandshakeCallback(rv);
}

int SSLServerSocket::DoPayloadRead() {
  DCHECK(completed_handshake_);
  DCHECK_EQ(STATE_NONE, next_handshake_state_);
  DCHECK(user_read_buf_);
  DCHECK_GT(user_read_buf_len_, 0);

  crypto::OpenSSLErrStackTracer err_tracer(FROM_HERE);
  int rv = SSL_read(ssl_.get(), user_read_buf_->data(), user_read_buf_len_);
  if (rv >= 0) {
    if (SSL_in_early_data(ssl_.get())) early_data_received_ = true;
    return rv;
  }
  int ssl_error = SSL_get_error(ssl_.get(), rv);
  net::OpenSSLErrorInfo error_info;
  int net_error =
      MapOpenSSLErrorWithDetails(ssl_error, err_tracer, &error_info);
  return net_error;
}

int SSLServerSocket::DoPayloadWrite() {
  DCHECK(completed_handshake_);
  DCHECK_EQ(STATE_NONE, next_handshake_state_);
  DCHECK(user_write_buf_);

  crypto::OpenSSLErrStackTracer err_tracer(FROM_HERE);
  int rv = SSL_write(ssl_.get(), user_write_buf_->data(), user_write_buf_len_);
  if (rv >= 0) return rv;
  int ssl_error = SSL_get_error(ssl_.get(), rv);
  net::OpenSSLErrorInfo error_info;
  int net_error =
      net::MapOpenSSLErrorWithDetails(ssl_error, err_tracer, &error_info);
  return net_error;
}

int SSLServerSocket::DoHandshakeLoop(int last_io_result) {
  int rv = last_io_result;
  do {
    // Default to STATE_NONE for next state.
    // (This is a quirk carried over from the windows
    // implementation.  It makes reading the logs a bit harder.)
    // State handlers can and often do call GotoState just
    // to stay in the current state.
    State state = next_handshake_state_;
    GotoState(STATE_NONE);
    switch (state) {
      case STATE_HANDSHAKE:
        rv = DoHandshake();
        break;
      case STATE_NONE:
      default:
        rv = net::ERR_UNEXPECTED;
        LOG(DFATAL) << "unexpected state " << state;
        break;
    }
  } while (rv != net::ERR_IO_PENDING && next_handshake_state_ != STATE_NONE);
  return rv;
}

int SSLServerSocket::DoHandshake() {
  crypto::OpenSSLErrStackTracer err_tracer(FROM_HERE);
  int net_error = net::OK;
  int rv = SSL_do_handshake(ssl_.get());
  if (rv == 1) {
    completed_handshake_ = true;
  } else {
    int ssl_error = SSL_get_error(ssl_.get(), rv);

    if (ssl_error == SSL_ERROR_WANT_PRIVATE_KEY_OPERATION) {
      // DCHECK(context_->private_key_);
      GotoState(STATE_HANDSHAKE);
      return net::ERR_IO_PENDING;
    }

    net::OpenSSLErrorInfo error_info;
    net_error =
        net::MapOpenSSLErrorWithDetails(ssl_error, err_tracer, &error_info);

    // SSL_R_CERTIFICATE_VERIFY_FAILED's mapping is different between client and
    // server.
    if (ERR_GET_LIB(error_info.error_code) == ERR_LIB_SSL &&
        ERR_GET_REASON(error_info.error_code) ==
            SSL_R_CERTIFICATE_VERIFY_FAILED) {
      net_error = net::ERR_BAD_SSL_CLIENT_AUTH_CERT;
    }

    // If not done, stay in this state
    if (net_error == net::ERR_IO_PENDING) {
      GotoState(STATE_HANDSHAKE);
    } else {
      LOG(ERROR) << "handshake failed; returned " << rv << ", SSL error code "
                 << ssl_error << ", net_error " << net_error;
    }
  }
  return net_error;
}

void SSLServerSocket::DoHandshakeCallback(int rv) {
  DCHECK_NE(rv, net::ERR_IO_PENDING);
  rv = rv > net::OK ? net::OK : rv;
  Socket::OnConnect(rv);
}

void SSLServerSocket::DoReadCallback(int rv) {
  DCHECK(rv != net::ERR_IO_PENDING);
  DCHECK(!user_read_callback_.is_null());

  user_read_buf_ = NULL;
  user_read_buf_len_ = 0;
  std::move(user_read_callback_).Run(rv);
}

void SSLServerSocket::DoWriteCallback(int rv) {
  DCHECK(rv != net::ERR_IO_PENDING);
  DCHECK(!user_write_callback_.is_null());

  user_write_buf_ = NULL;
  user_write_buf_len_ = 0;
  std::move(user_write_callback_).Run(rv);
}

void SSLServerSocket::OnWrite(int result) {
  if (result == ::net::ERR_CONNECTION_RESET) {
    stream_socket_.reset();
  }
  Socket::OnWrite(result);
}

void SSLServerSocket::OnRead(int result) {
  if (result == 0) {
    result = ::net::ERR_CONNECTION_CLOSED;
    stream_socket_.reset();
  }
  Socket::OnRead(result);
}

int SSLServerSocket::Init() {
  static const int kBufferSize = 17 * 1024;

  crypto::OpenSSLErrStackTracer err_tracer(FROM_HERE);

  if (!ssl_ ||
      !SSL_set_ex_data(ssl_.get(),
                       SocketDataIndex::GetInstance()->ssl_socket_data_index_,
                       this))
    return net::ERR_UNEXPECTED;

  transport_adapter_.reset(new SocketBIOAdapter(
      stream_socket_.get(), kBufferSize, kBufferSize, this));
  BIO* transport_bio = transport_adapter_->bio();

  BIO_up_ref(transport_bio);  // SSL_set0_rbio takes ownership.
  SSL_set0_rbio(ssl_.get(), transport_bio);

  BIO_up_ref(transport_bio);  // SSL_set0_wbio takes ownership.
  SSL_set0_wbio(ssl_.get(), transport_bio);

  return net::OK;
}

}  // namespace felicia

#undef GotoState

#endif  // !defined(FEL_NO_SSL)