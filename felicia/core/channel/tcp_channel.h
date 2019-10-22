#ifndef FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/settings.h"
#include "felicia/core/channel/socket/ssl_server_socket.h"
#include "felicia/core/channel/socket/tcp_server_socket.h"

namespace felicia {

class EXPORT TCPChannel : public Channel {
 public:
  using AcceptOnceInterceptCallback =
      base::OnceCallback<void(StatusOr<std::unique_ptr<TCPChannel>>)>;

  ~TCPChannel() override;

  bool IsTCPChannel() const override;

  ChannelDef::Type type() const override;

  bool HasReceivers() const override;

  StatusOr<ChannelDef> Listen();

  void AcceptLoop(TCPServerSocket::AcceptCallback callback);

  void AcceptOnceIntercept(AcceptOnceInterceptCallback callback);

  void AddClientChannel(std::unique_ptr<TCPChannel> channel);

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override;

 private:
  friend class ChannelFactory;

  explicit TCPChannel(
      const channel::TCPSettings& settings = channel::TCPSettings());

  void DoAcceptLoop();

  // This is callback called from AcceptLoop
  void OnAccept(StatusOr<std::unique_ptr<net::TCPSocket>> status_or);

#if !defined(FEL_NO_SSL)
  void OnSSLHandshake(Status s);
#endif

  void OnConnect(StatusOnceCallback callback, Status s);

  channel::TCPSettings settings_;
  AcceptOnceInterceptCallback accept_once_intercept_callback_;
  TCPServerSocket::AcceptCallback accept_callback_;

#if !defined(FEL_NO_SSL)
  std::unique_ptr<SSLServerSocket> ssl_server_socket_;
#endif

  DISALLOW_COPY_AND_ASSIGN(TCPChannel);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_