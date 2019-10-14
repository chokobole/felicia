#if !defined(FEL_NO_SSL)

#include "felicia/core/channel/socket/ssl_server_context.h"

#include "openssl/ssl.h"

#include "third_party/chromium/base/memory/ptr_util.h"

#include "felicia/core/channel/socket/ssl_server_socket.h"

namespace felicia {

SSLServerContext::~SSLServerContext() = default;

// static
std::unique_ptr<SSLServerContext> SSLServerContext::NewSSLServerContext(
    const base::FilePath& cert_file_path,
    const base::FilePath& private_key_file_path) {
  return base::WrapUnique(
      new SSLServerContext(cert_file_path, private_key_file_path));
}

std::unique_ptr<SSLServerSocket> SSLServerContext::CreateSSLServerSocket(
    std::unique_ptr<StreamSocket> stream_socket) {
  return std::make_unique<SSLServerSocket>(this, std::move(stream_socket));
}

SSLServerContext::SSLServerContext(
    const base::FilePath& cert_file_path,
    const base::FilePath& private_key_file_path) {
  crypto::EnsureOpenSSLInit();
  ssl_ctx_.reset(SSL_CTX_new(TLS_server_method()));
  SSL_CTX_set_session_cache_mode(ssl_ctx_.get(), SSL_SESS_CACHE_SERVER);
  uint8_t session_ctx_id = 0;
  SSL_CTX_set_session_id_context(ssl_ctx_.get(), &session_ctx_id,
                                 sizeof(session_ctx_id));

  SSL_CTX_set_early_data_enabled(ssl_ctx_.get(), false);
  CHECK(SSL_CTX_set_min_proto_version(ssl_ctx_.get(), TLS1_2_VERSION));
  CHECK(SSL_CTX_set_max_proto_version(ssl_ctx_.get(), TLS1_3_VERSION));

  std::string cert_file_path_str = cert_file_path.AsUTF8Unsafe();
  CHECK(SSL_CTX_use_certificate_file(ssl_ctx_.get(), cert_file_path_str.c_str(),
                                     SSL_FILETYPE_PEM));
  std::string private_key_file_path_str = private_key_file_path.AsUTF8Unsafe();
  CHECK(SSL_CTX_use_PrivateKey_file(
      ssl_ctx_.get(), private_key_file_path_str.c_str(), SSL_FILETYPE_PEM));
  // OpenSSL defaults some options to on, others to off. To avoid ambiguity,
  // set everything we care about to an absolute value.
  net::SslSetClearMask options;
  options.ConfigureFlag(SSL_OP_NO_COMPRESSION, true);

  SSL_CTX_set_options(ssl_ctx_.get(), options.set_mask);
  SSL_CTX_clear_options(ssl_ctx_.get(), options.clear_mask);

  // Same as above, this time for the SSL mode.
  net::SslSetClearMask mode;

  mode.ConfigureFlag(SSL_MODE_RELEASE_BUFFERS, true);

  SSL_CTX_set_mode(ssl_ctx_.get(), mode.set_mask);
  SSL_CTX_clear_mode(ssl_ctx_.get(), mode.clear_mask);
}

}  // namespace felicia

#endif  // !defined(FEL_NO_SSL)