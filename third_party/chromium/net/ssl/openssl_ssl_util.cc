// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/ssl/openssl_ssl_util.h"

#include <errno.h>
#include <utility>

#include "base/bind.h"
#include "base/callback.h"
#include "base/lazy_instance.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/values.h"
#include "build/build_config.h"
#include "crypto/openssl_util.h"
#include "net/base/net_errors.h"
#include "openssl/err.h"
#include "openssl/ssl.h"

namespace net {

SslSetClearMask::SslSetClearMask()
    : set_mask(0),
      clear_mask(0) {
}

void SslSetClearMask::ConfigureFlag(long flag, bool state) {
  (state ? set_mask : clear_mask) |= flag;
  // Make sure we haven't got any intersection in the set & clear options.
  DCHECK_EQ(0, set_mask & clear_mask) << flag << ":" << state;
}

namespace {

class OpenSSLNetErrorLibSingleton {
 public:
  OpenSSLNetErrorLibSingleton() {
    crypto::EnsureOpenSSLInit();

    // Allocate a new error library value for inserting net errors into
    // OpenSSL. This does not register any ERR_STRING_DATA for the errors, so
    // stringifying error codes through OpenSSL will return NULL.
    net_error_lib_ = ERR_get_next_error_library();
  }

  int net_error_lib() const { return net_error_lib_; }

 private:
  int net_error_lib_;
};

base::LazyInstance<OpenSSLNetErrorLibSingleton>::Leaky g_openssl_net_error_lib =
    LAZY_INSTANCE_INITIALIZER;

int OpenSSLNetErrorLib() {
  return g_openssl_net_error_lib.Get().net_error_lib();
}

int MapOpenSSLErrorSSL(uint32_t error_code) {
  DCHECK_EQ(ERR_LIB_SSL, ERR_GET_LIB(error_code));

#if DCHECK_IS_ON()
  char buf[ERR_ERROR_STRING_BUF_LEN];
  ERR_error_string_n(error_code, buf, sizeof(buf));
  DVLOG(1) << "OpenSSL SSL error, reason: " << ERR_GET_REASON(error_code)
           << ", name: " << buf;
#endif

  switch (ERR_GET_REASON(error_code)) {
    case SSL_R_READ_TIMEOUT_EXPIRED:
      return ERR_TIMED_OUT;
    case SSL_R_UNKNOWN_CERTIFICATE_TYPE:
    case SSL_R_UNKNOWN_CIPHER_TYPE:
    case SSL_R_UNKNOWN_KEY_EXCHANGE_TYPE:
    case SSL_R_UNKNOWN_SSL_VERSION:
      return ERR_NOT_IMPLEMENTED;
    case SSL_R_NO_CIPHER_MATCH:
    case SSL_R_NO_SHARED_CIPHER:
    case SSL_R_TLSV1_ALERT_INSUFFICIENT_SECURITY:
    case SSL_R_TLSV1_ALERT_PROTOCOL_VERSION:
    case SSL_R_UNSUPPORTED_PROTOCOL:
      return ERR_SSL_VERSION_OR_CIPHER_MISMATCH;
    case SSL_R_SSLV3_ALERT_BAD_CERTIFICATE:
    case SSL_R_SSLV3_ALERT_UNSUPPORTED_CERTIFICATE:
    case SSL_R_SSLV3_ALERT_CERTIFICATE_REVOKED:
    case SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED:
    case SSL_R_SSLV3_ALERT_CERTIFICATE_UNKNOWN:
    case SSL_R_TLSV1_ALERT_ACCESS_DENIED:
    case SSL_R_TLSV1_ALERT_UNKNOWN_CA:
    case SSL_R_TLSV1_CERTIFICATE_REQUIRED:
      return ERR_BAD_SSL_CLIENT_AUTH_CERT;
    case SSL_R_SSLV3_ALERT_DECOMPRESSION_FAILURE:
      return ERR_SSL_DECOMPRESSION_FAILURE_ALERT;
    case SSL_R_SSLV3_ALERT_BAD_RECORD_MAC:
      return ERR_SSL_BAD_RECORD_MAC_ALERT;
    case SSL_R_TLSV1_ALERT_DECRYPT_ERROR:
      return ERR_SSL_DECRYPT_ERROR_ALERT;
    case SSL_R_TLSV1_UNRECOGNIZED_NAME:
      return ERR_SSL_UNRECOGNIZED_NAME_ALERT;
    case SSL_R_BAD_DH_P_LENGTH:
      return ERR_SSL_WEAK_SERVER_EPHEMERAL_DH_KEY;
    case SSL_R_SERVER_CERT_CHANGED:
      return ERR_SSL_SERVER_CERT_CHANGED;
    case SSL_R_WRONG_VERSION_ON_EARLY_DATA:
      return ERR_WRONG_VERSION_ON_EARLY_DATA;
    case SSL_R_TLS13_DOWNGRADE:
      return ERR_TLS13_DOWNGRADE_DETECTED;
    // SSL_R_SSLV3_ALERT_HANDSHAKE_FAILURE may be returned from the server after
    // receiving ClientHello if there's no common supported cipher. Map that
    // specific case to ERR_SSL_VERSION_OR_CIPHER_MISMATCH to match the NSS
    // implementation. See https://goo.gl/oMtZW and https://crbug.com/446505.
    case SSL_R_SSLV3_ALERT_HANDSHAKE_FAILURE: {
      uint32_t previous = ERR_peek_error();
      if (previous != 0 && ERR_GET_LIB(previous) == ERR_LIB_SSL &&
          ERR_GET_REASON(previous) == SSL_R_HANDSHAKE_FAILURE_ON_CLIENT_HELLO) {
        return ERR_SSL_VERSION_OR_CIPHER_MISMATCH;
      }
      return ERR_SSL_PROTOCOL_ERROR;
    }
    default:
      return ERR_SSL_PROTOCOL_ERROR;
  }
}

}  // namespace

void OpenSSLPutNetError(const base::Location& location, int err) {
  // Net error codes are negative. Encode them as positive numbers.
  err = -err;
  if (err < 0 || err > 0xfff) {
    // OpenSSL reserves 12 bits for the reason code.
    NOTREACHED();
    err = ERR_INVALID_ARGUMENT;
  }
  ERR_put_error(OpenSSLNetErrorLib(), 0 /* unused */, err, location.file_name(),
                location.line_number());
}

int MapOpenSSLError(int err, const crypto::OpenSSLErrStackTracer& tracer) {
  OpenSSLErrorInfo error_info;
  return MapOpenSSLErrorWithDetails(err, tracer, &error_info);
}

int MapOpenSSLErrorWithDetails(int err,
                               const crypto::OpenSSLErrStackTracer& tracer,
                               OpenSSLErrorInfo* out_error_info) {
  *out_error_info = OpenSSLErrorInfo();

  switch (err) {
    case SSL_ERROR_WANT_READ:
    case SSL_ERROR_WANT_WRITE:
      return ERR_IO_PENDING;
    case SSL_ERROR_EARLY_DATA_REJECTED:
      return ERR_EARLY_DATA_REJECTED;
    case SSL_ERROR_SYSCALL:
      PLOG(ERROR) << "OpenSSL SYSCALL error, earliest error code in "
                     "error queue: "
                  << ERR_peek_error();
      return ERR_FAILED;
    case SSL_ERROR_SSL:
      // Walk down the error stack to find an SSL or net error.
      while (true) {
        OpenSSLErrorInfo error_info;
        error_info.error_code =
            ERR_get_error_line(&error_info.file, &error_info.line);
        if (error_info.error_code == 0) {
          // Map errors to ERR_SSL_PROTOCOL_ERROR by default, reporting the most
          // recent error in |*out_error_info|.
          return ERR_SSL_PROTOCOL_ERROR;
        }

        *out_error_info = error_info;
        if (ERR_GET_LIB(error_info.error_code) == ERR_LIB_SSL) {
          return MapOpenSSLErrorSSL(error_info.error_code);
        }
        if (ERR_GET_LIB(error_info.error_code) == OpenSSLNetErrorLib()) {
          // Net error codes are negative but encoded in OpenSSL as positive
          // numbers.
          return -ERR_GET_REASON(error_info.error_code);
        }
      }
    default:
      // TODO(joth): Implement full mapping.
      LOG(WARNING) << "Unknown OpenSSL error " << err;
      return ERR_SSL_PROTOCOL_ERROR;
  }
}

}  // namespace net
