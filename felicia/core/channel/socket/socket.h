#ifndef FELICIA_CORE_CHANNEL_SOCKET_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_SOCKET_H_

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/build/build_config.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/channel/channel_impl.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class TCPSocket;
class UDPSocket;
class WebSocket;
class UnixDomainSocket;

class Socket : public ChannelImpl {
 public:
  Socket();
  virtual ~Socket();

  bool IsSocket() const override;

  virtual bool IsClient() const;
  virtual bool IsServer() const;

  virtual bool IsTCPSocket() const;
  virtual bool IsUDPSocket() const;
  virtual bool IsWebSocket() const;
#if defined(OS_POSIX)
  virtual bool IsUnixDomainSocket() const;
#endif

  TCPSocket* ToTCPSocket();
  UDPSocket* ToUDPSocket();
  WebSocket* ToWebSocket();
#if defined(OS_POSIX)
  UnixDomainSocket* ToUnixDomainSocket();
#endif

 protected:
  friend class SocketBroadcaster;
  friend class WebSocketChannelBroadcaster;

  static void CallbackWithStatus(StatusOnceCallback callback, int result);

  StatusOnceCallback write_callback_;
  StatusOnceCallback read_callback_;

  DISALLOW_COPY_AND_ASSIGN(Socket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_SOCKET_H_