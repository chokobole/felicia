// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/uds_channel.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/memory/ptr_util.h"

#include "felicia/core/channel/socket/unix_domain_client_socket.h"

namespace felicia {

UDSChannel::UDSChannel(const channel::UDSSettings& settings)
    : settings_(settings) {}

UDSChannel::~UDSChannel() = default;

bool UDSChannel::IsUDSChannel() const { return true; }

ChannelDef::Type UDSChannel::type() const {
  return ChannelDef::CHANNEL_TYPE_UDS;
}

bool UDSChannel::HasReceivers() const {
  DCHECK(channel_impl_);
  UnixDomainServerSocket* server_socket = channel_impl_->ToSocket()
                                              ->ToUnixDomainSocket()
                                              ->ToUnixDomainServerSocket();
  return server_socket->accepted_sockets().size() > 0;
}

StatusOr<ChannelDef> UDSChannel::BindAndListen() {
  DCHECK(!channel_impl_);
  channel_impl_ = std::make_unique<UnixDomainServerSocket>();
  UnixDomainServerSocket* server_socket = channel_impl_->ToSocket()
                                              ->ToUnixDomainSocket()
                                              ->ToUnixDomainServerSocket();
  return server_socket->BindAndListen();
}

void UDSChannel::AcceptLoop(
    UnixDomainServerSocket::AcceptCallback accept_callback) {
  DCHECK(channel_impl_);
  DCHECK(!accept_callback.is_null());
  UnixDomainServerSocket* server_socket = channel_impl_->ToSocket()
                                              ->ToUnixDomainSocket()
                                              ->ToUnixDomainServerSocket();
  server_socket->AcceptLoop(accept_callback, settings_.auth_callback);
}

void UDSChannel::AcceptOnceIntercept(
    AcceptOnceInterceptCallback accept_once_intercept_callback) {
  DCHECK(channel_impl_);
  DCHECK(!accept_once_intercept_callback.is_null());
  UnixDomainServerSocket* server_socket = channel_impl_->ToSocket()
                                              ->ToUnixDomainSocket()
                                              ->ToUnixDomainServerSocket();
  server_socket->AcceptOnceIntercept(
      base::BindOnce(&UDSChannel::OnAccept, base::Unretained(this),
                     std::move(accept_once_intercept_callback)),
      settings_.auth_callback);
}

void UDSChannel::OnAccept(
    AcceptOnceInterceptCallback callback,
    StatusOr<std::unique_ptr<net::SocketPosix>> status_or) {
  if (status_or.ok()) {
    auto channel = base::WrapUnique(new UDSChannel());
    channel->channel_impl_ = std::make_unique<UnixDomainClientSocket>(
        std::move(status_or).ValueOrDie());
    std::move(callback).Run(std::move(channel));
  } else {
    std::move(callback).Run(status_or.status());
  }
}

void UDSChannel::Connect(const ChannelDef& channel_def,
                         StatusOnceCallback callback) {
  DCHECK(!channel_impl_);
  DCHECK(!callback.is_null());
  net::UDSEndPoint uds_endpoint;
  Status s = ToNetUDSEndPoint(channel_def, &uds_endpoint);
  if (!s.ok()) {
    std::move(callback).Run(s);
    return;
  }
  channel_impl_ = std::make_unique<UnixDomainClientSocket>();
  UnixDomainClientSocket* client_socket = channel_impl_->ToSocket()
                                              ->ToUnixDomainSocket()
                                              ->ToUnixDomainClientSocket();
  client_socket->Connect(uds_endpoint, std::move(callback));
}

}  // namespace felicia