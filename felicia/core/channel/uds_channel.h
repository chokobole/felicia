#ifndef FELICIA_CORE_CHANNEL_UDS_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_UDS_CHANNEL_H_

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/socket/unix_domain_client_socket.h"
#include "felicia/core/channel/socket/unix_domain_server_socket.h"
#include "felicia/core/lib/error/statusor.h"

namespace felicia {

template <typename MessageTy>
class UDSChannel : public Channel<MessageTy> {
 public:
  using AcceptOnceInterceptCallback = ::base::OnceCallback<void(
      StatusOr<std::unique_ptr<UDSChannel<MessageTy>>>)>;

  UDSChannel();
  ~UDSChannel();

  bool IsUDSChannel() const override { return true; }

  ChannelDef::Type type() const override {
    return ChannelDef::CHANNEL_TYPE_UDS;
  }

  bool HasReceivers() const override;

  StatusOr<ChannelDef> BindAndListen();

  void AcceptLoop(UnixDomainServerSocket::AcceptCallback accept_callback,
                  UnixDomainServerSocket::AuthCallback auth_callback);

  void AcceptOnceIntercept(
      AcceptOnceInterceptCallback accept_once_intercept_callback,
      UnixDomainServerSocket::AuthCallback auth_callback);

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override;

 private:
  void OnAccept(AcceptOnceInterceptCallback callback,
                StatusOr<std::unique_ptr<::net::SocketPosix>> status_or);

  DISALLOW_COPY_AND_ASSIGN(UDSChannel);
};

template <typename MessageTy>
UDSChannel<MessageTy>::UDSChannel() = default;

template <typename MessageTy>
UDSChannel<MessageTy>::~UDSChannel() = default;

template <typename MessageTy>
bool UDSChannel<MessageTy>::HasReceivers() const {
  DCHECK(this->channel_impl_);
  UnixDomainServerSocket* server_socket = this->channel_impl_->ToSocket()
                                              ->ToUnixDomainSocket()
                                              ->ToUnixDomainServerSocket();
  return server_socket->accepted_sockets().size() > 0;
}

template <typename MessageTy>
StatusOr<ChannelDef> UDSChannel<MessageTy>::BindAndListen() {
  DCHECK(!this->channel_impl_);
  this->channel_impl_ = std::make_unique<UnixDomainServerSocket>();
  UnixDomainServerSocket* server_socket = this->channel_impl_->ToSocket()
                                              ->ToUnixDomainSocket()
                                              ->ToUnixDomainServerSocket();
  return server_socket->BindAndListen();
}

template <typename MessageTy>
void UDSChannel<MessageTy>::AcceptLoop(
    UnixDomainServerSocket::AcceptCallback accept_callback,
    UnixDomainServerSocket::AuthCallback auth_callback) {
  DCHECK(this->channel_impl_);
  DCHECK(!accept_callback.is_null());
  UnixDomainServerSocket* server_socket = this->channel_impl_->ToSocket()
                                              ->ToUnixDomainSocket()
                                              ->ToUnixDomainServerSocket();
  server_socket->AcceptLoop(accept_callback, auth_callback);
}

template <typename MessageTy>
void UDSChannel<MessageTy>::AcceptOnceIntercept(
    AcceptOnceInterceptCallback accept_once_intercept_callback,
    UnixDomainServerSocket::AuthCallback auth_callback) {
  DCHECK(this->channel_impl_);
  DCHECK(!accept_once_intercept_callback.is_null());
  UnixDomainServerSocket* server_socket = this->channel_impl_->ToSocket()
                                              ->ToUnixDomainSocket()
                                              ->ToUnixDomainServerSocket();
  server_socket->AcceptOnceIntercept(::base::BindOnce(
      &UDSChannel<MessageTy>::OnAccept, ::base::Unretained(this),
      std::move(accept_once_intercept_callback), auth_callback));
}

template <typename MessageTy>
void UDSChannel<MessageTy>::OnAccept(
    AcceptOnceInterceptCallback callback,
    StatusOr<std::unique_ptr<::net::SocketPosix>> status_or) {
  if (status_or.ok()) {
    auto channel = std::make_unique<UDSChannel<MessageTy>>();
    channel->channel_impl_ = std::make_unique<UnixDomainClientSocket>(
        std::move(status_or.ValueOrDie()));
    std::move(callback).Run(std::move(channel));
  } else {
    std::move(callback).Run(status_or.status());
  }
}

template <typename MessageTy>
void UDSChannel<MessageTy>::Connect(const ChannelDef& channel_def,
                                    StatusOnceCallback callback) {
  DCHECK(!this->channel_impl_);
  DCHECK(!callback.is_null());
  ::net::UDSEndPoint uds_endpoint;
  Status s = ToNetUDSEndPoint(channel_def, &uds_endpoint);
  if (!s.ok()) {
    std::move(callback).Run(s);
    return;
  }
  this->channel_impl_ = std::make_unique<UnixDomainClientSocket>();
  UnixDomainClientSocket* client_socket = this->channel_impl_->ToSocket()
                                              ->ToUnixDomainSocket()
                                              ->ToUnixDomainClientSocket();
  client_socket->Connect(uds_endpoint, std::move(callback));
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_UDS_CHANNEL_H_