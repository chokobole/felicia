#ifndef FELICIA_CORE_CHANNEL_SOCKET_UDP_CLIENT_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_UDP_CLIENT_SOCKET_H_

#include "felicia/core/channel/socket/udp_socket.h"

namespace felicia {

class UDPClientSocket : public UDPSocket {
 public:
  UDPClientSocket();
  ~UDPClientSocket();

  bool IsClient() const override;

  void Connect(const ::net::IPEndPoint& ip_endpoint,
               StatusOnceCallback callback);

  void Write(scoped_refptr<::net::IOBuffer> buffer, int size,
             StatusOnceCallback callback) override;
  void Read(scoped_refptr<::net::GrowableIOBuffer> buffer, int size,
            StatusOnceCallback callback) override;

 private:
  std::unique_ptr<::net::UDPSocket> socket_;
  ::net::IPEndPoint multicast_ip_endpoint_;
  ::net::IPEndPoint recv_from_ip_endpoint_;

  DISALLOW_COPY_AND_ASSIGN(UDPClientSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_UDP_CLIENT_SOCKET_H_