#ifndef FELICIA_CORE_CHANNEL_UDP_SERVER_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_UDP_SERVER_CHANNEL_H_

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/base/ip_endpoint.h"
#include "third_party/chromium/net/socket/udp_socket.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/udp_channel_base.h"

namespace felicia {

class EXPORT UDPServerChannel : public UDPChannelBase {
 public:
  UDPServerChannel();
  ~UDPServerChannel();

  bool IsServer() const override;

  StatusOr<ChannelSource> Bind();

  void Write(char* buffer, int size, StatusCallback callback) override;
  void Read(char* buffer, int size, StatusCallback callback) override;

 private:
  std::unique_ptr<::net::UDPSocket> socket_;
  ::net::IPEndPoint multicast_ip_endpoint_;
  ::net::IPEndPoint recv_from_ip_endpoint_;

  DISALLOW_COPY_AND_ASSIGN(UDPServerChannel);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_UDP_SERVER_CHANNEL_H_