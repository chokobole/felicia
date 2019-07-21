#ifndef FELICIA_CORE_CHANNEL_SOCKET_UNIX_DOMAIN_CLIENT_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_UNIX_DOMAIN_CLIENT_SOCKET_H_

#include "third_party/chromium/net/socket/socket_posix.h"

#include "felicia/core/channel/socket/uds_endpoint.h"
#include "felicia/core/channel/socket/unix_domain_socket.h"

namespace felicia {

class UnixDomainClientSocket : public UnixDomainSocket {
 public:
  UnixDomainClientSocket();
  explicit UnixDomainClientSocket(std::unique_ptr<::net::SocketPosix> socket);
  ~UnixDomainClientSocket();

  int socket_fd() const;

  void Connect(const ::net::UDSEndPoint& uds_endpoint,
               StatusOnceCallback callback);

  // Socket methods
  bool IsClient() const override;
  bool IsConnected() const override;

  // ChannelImpl methods
  void Write(scoped_refptr<::net::IOBuffer> buffer, int size,
             StatusOnceCallback callback) override;
  void Read(scoped_refptr<::net::GrowableIOBuffer> buffer, int size,
            StatusOnceCallback callback) override;

  DISALLOW_COPY_AND_ASSIGN(UnixDomainClientSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_UNIX_DOMAIN_CLIENT_SOCKET_H_