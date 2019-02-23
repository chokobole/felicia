#ifndef FELICIA_CORE_CHANNEL_UDP_CLIENT_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_UDP_CLIENT_CHANNEL_H_

#include <memory>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/base/ip_endpoint.h"
#include "third_party/chromium/net/socket/udp_socket.h"

#include "felicia/core/channel/udp_channel_base.h"

namespace felicia {

class UDPClientChannel : public UDPChannelBase {
 public:
  UDPClientChannel();
  ~UDPClientChannel();

  bool IsClient() const override { return true; }

  void Connect(const ::net::IPEndPoint& ip_endpoint, StatusCallback callback);

  void Write(::net::IOBuffer* buf, size_t buf_len,
             StatusCallback callback) override;
  void Read(::net::IOBuffer* buf, size_t buf_len,
            StatusCallback callback) override;

 private:
  std::unique_ptr<::net::UDPSocket> socket_;
  ::net::IPEndPoint multicast_ip_endpoint_;
  ::net::IPEndPoint recv_from_ip_endpoint_;

  DISALLOW_COPY_AND_ASSIGN(UDPClientChannel);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_UDP_CLIENT_CHANNEL_H_