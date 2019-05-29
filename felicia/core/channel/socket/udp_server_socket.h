#ifndef FELICIA_CORE_CHANNEL_SOCKET_UDP_SERVER_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_UDP_SERVER_SOCKET_H_

#include "felicia/core/channel/socket/udp_socket.h"

#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

class EXPORT UDPServerSocket : public UDPSocket {
 public:
  UDPServerSocket();
  ~UDPServerSocket();

  bool IsServer() const override;

  StatusOr<ChannelDef> Bind();

  void Write(scoped_refptr<::net::IOBuffer> buffer, int size,
             StatusOnceCallback callback) override;
  void Read(scoped_refptr<::net::GrowableIOBuffer> buffer, int size,
            StatusOnceCallback callback) override;

 private:
  std::unique_ptr<::net::UDPSocket> socket_;
  ::net::IPEndPoint multicast_ip_endpoint_;
  ::net::IPEndPoint recv_from_ip_endpoint_;

  DISALLOW_COPY_AND_ASSIGN(UDPServerSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_UDP_SERVER_SOCKET_H_