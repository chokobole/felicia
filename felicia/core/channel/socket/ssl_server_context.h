#if !defined(FEL_NO_SSL)

#ifndef FELICIA_CORE_CHANNEL_SOCKET_SSL_SERVER_CONTEXT_H_
#define FELICIA_CORE_CHANNEL_SOCKET_SSL_SERVER_CONTEXT_H_

#include "openssl/base.h"

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/channel/socket/stream_socket.h"
#include "felicia/core/lib/base/export.h"

namespace felicia {

class SSLServerSocket;
template <typename T>
class TCPChannel;

class EXPORT SSLServerContext {
 public:
  ~SSLServerContext();

  static std::unique_ptr<SSLServerContext> NewSSLServerContext(
      const base::FilePath& cert_file_path,
      const base::FilePath& private_key_file_path);

 private:
  friend class SSLServerSocket;
  template <typename T>
  friend class TCPChannel;
  SSLServerContext(const base::FilePath& cert_file_path,
                   const base::FilePath& private_key_file_path);

  std::unique_ptr<SSLServerSocket> CreateSSLServerSocket(
      std::unique_ptr<StreamSocket> stream_socket);

  bssl::UniquePtr<SSL_CTX> ssl_ctx_;

  DISALLOW_COPY_AND_ASSIGN(SSLServerContext);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_SSL_SERVER_CONTEXT_H_

#endif  // !defined(FEL_NO_SSL)