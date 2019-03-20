#ifndef FELICIA_CORE_CHANNEL_TCP_CLIENT_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_TCP_CLIENT_CHANNEL_H_

#include <memory>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/base/ip_endpoint.h"
#include "third_party/chromium/net/socket/tcp_socket.h"

#include "felicia/core/channel/tcp_channel_base.h"

namespace felicia {

class EXPORT TCPClientChannel : public TCPChannelBase {
 public:
  TCPClientChannel();
  ~TCPClientChannel();

  bool IsClient() const override;

  bool IsConnected() const override;

  void Connect(const ::net::IPEndPoint& ip_endpoint, StatusCallback callback);

  void Write(::net::IOBufferWithSize* buffer, StatusCallback callback) override;
  void Read(::net::IOBufferWithSize* buffer, StatusCallback callback) override;

 private:
  void OnConnect(int result);

  void OnWrite(int result);
  void OnRead(int result);

  StatusCallback connect_callback_;

  std::unique_ptr<::net::TCPSocket> socket_;

  DISALLOW_COPY_AND_ASSIGN(TCPClientChannel);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_TCP_CLIENT_CHANNEL_H_