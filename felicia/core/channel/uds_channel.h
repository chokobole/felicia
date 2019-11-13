// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_UDS_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_UDS_CHANNEL_H_

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/settings.h"
#include "felicia/core/channel/socket/unix_domain_server_socket.h"
#include "felicia/core/lib/error/statusor.h"

namespace felicia {

class FEL_EXPORT UDSChannel : public Channel {
 public:
  using AcceptOnceInterceptCallback =
      base::OnceCallback<void(StatusOr<std::unique_ptr<UDSChannel>>)>;

  ~UDSChannel() override;

  bool IsUDSChannel() const override;

  ChannelDef::Type type() const override;

  bool HasReceivers() const override;

  StatusOr<ChannelDef> BindAndListen();

  void AcceptLoop(UnixDomainServerSocket::AcceptCallback accept_callback);

  void AcceptOnceIntercept(
      AcceptOnceInterceptCallback accept_once_intercept_callback);

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override;

 private:
  friend class ChannelFactory;

  explicit UDSChannel(
      const channel::UDSSettings& settings = channel::UDSSettings());

  void OnAccept(AcceptOnceInterceptCallback callback,
                StatusOr<std::unique_ptr<net::SocketPosix>> status_or);

  channel::UDSSettings settings_;

  DISALLOW_COPY_AND_ASSIGN(UDSChannel);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_UDS_CHANNEL_H_