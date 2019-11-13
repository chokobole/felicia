// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_WS_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_WS_CHANNEL_H_

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/settings.h"
#include "felicia/core/channel/socket/web_socket_server.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class FEL_EXPORT WSChannel : public Channel {
 public:
  ~WSChannel() override;

  bool IsWSChannel() const override;

  ChannelDef::Type type() const override;

  bool HasNativeHeader() const override;

  bool HasReceivers() const override;

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override;

  StatusOr<ChannelDef> Listen();

  void AcceptLoop(TCPServerSocket::AcceptCallback accept_callback);

 private:
  friend class ChannelFactory;

  explicit WSChannel(
      const channel::WSSettings& settings = channel::WSSettings());

  channel::WSSettings settings_;

  DISALLOW_COPY_AND_ASSIGN(WSChannel);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_WS_CHANNEL_H_