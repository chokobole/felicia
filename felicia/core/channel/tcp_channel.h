#ifndef FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_

#include <memory>
#include <utility>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/ip_endpoint.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/socket/tcp_client_socket.h"
#include "felicia/core/channel/socket/tcp_server_socket.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

template <typename MessageTy>
class TCPChannel : public Channel<MessageTy> {
 public:
  TCPChannel();
  ~TCPChannel();

  bool IsTCPChannel() const override { return true; }

  bool HasReceivers() const override;

  bool IsConnected() const {
    DCHECK(this->channel_impl_);
    return this->channel_impl_->ToSocket()->ToTCPSocket()->IsConnected();
  }

  StatusOr<ChannelDef> Listen();

  void DoAcceptLoop(TCPServerSocket::AcceptCallback accept_callback);

  void AcceptOnce(TCPServerSocket::AcceptOnceCallback accept_once_callback);

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(TCPChannel);
};

template <typename MessageTy>
TCPChannel<MessageTy>::TCPChannel() {}

template <typename MessageTy>
TCPChannel<MessageTy>::~TCPChannel() = default;

template <typename MessageTy>
bool TCPChannel<MessageTy>::HasReceivers() const {
  DCHECK(this->channel_impl_);
  TCPServerSocket* server_socket =
      this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  return server_socket->accepted_sockets().size() > 0;
}

template <typename MessageTy>
StatusOr<ChannelDef> TCPChannel<MessageTy>::Listen() {
  DCHECK(!this->channel_impl_);
  this->channel_impl_ = std::make_unique<TCPServerSocket>();
  TCPServerSocket* server_socket =
      this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  return server_socket->Listen();
}

template <typename MessageTy>
void TCPChannel<MessageTy>::DoAcceptLoop(
    TCPServerSocket::AcceptCallback accept_callback) {
  DCHECK(this->channel_impl_);
  DCHECK(!accept_callback.is_null());
  TCPServerSocket* server_socket =
      this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  server_socket->DoAcceptLoop(accept_callback);
}

template <typename MessageTy>
void TCPChannel<MessageTy>::AcceptOnce(
    TCPServerSocket::AcceptOnceCallback accept_once_callback) {
  DCHECK(this->channel_impl_);
  DCHECK(!accept_once_callback.is_null());
  TCPServerSocket* server_socket =
      this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  server_socket->AcceptOnce(std::move(accept_once_callback));
}

template <typename MessageTy>
void TCPChannel<MessageTy>::Connect(const ChannelDef& channel_def,
                                    StatusOnceCallback callback) {
  DCHECK(!this->channel_impl_);
  DCHECK(!callback.is_null());
  ::net::IPEndPoint ip_endpoint;
  bool ret = ToNetIPEndPoint(channel_def, &ip_endpoint);
  DCHECK(ret);
  auto client_socket = std::make_unique<TCPClientSocket>();
  client_socket->Connect(ip_endpoint, std::move(callback));
  this->channel_impl_ = std::move(client_socket);
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_