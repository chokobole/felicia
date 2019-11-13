// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/ws_channel.h"

#include "third_party/chromium/base/bind.h"

namespace felicia {

WSChannel::WSChannel(const channel::WSSettings& settings)
    : settings_(settings) {}

WSChannel::~WSChannel() = default;

bool WSChannel::IsWSChannel() const { return true; }

ChannelDef::Type WSChannel::type() const { return ChannelDef::CHANNEL_TYPE_WS; }

bool WSChannel::HasNativeHeader() const { return true; }

bool WSChannel::HasReceivers() const {
  DCHECK(channel_impl_);
  WebSocketServer* server =
      channel_impl_->ToSocket()->ToWebSocket()->ToWebSocketServer();
  return server->HasReceivers();
}

void WSChannel::Connect(const ChannelDef& channel_def,
                        StatusOnceCallback callback) {
  NOTREACHED();
}

StatusOr<ChannelDef> WSChannel::Listen() {
  DCHECK(!channel_impl_);
  channel_impl_ = std::make_unique<WebSocketServer>(settings_);
  WebSocketServer* server =
      channel_impl_->ToSocket()->ToWebSocket()->ToWebSocketServer();
  return server->Listen();
}

void WSChannel::AcceptLoop(TCPServerSocket::AcceptCallback accept_callback) {
  DCHECK(channel_impl_);
  DCHECK(!accept_callback.is_null());
  WebSocketServer* server =
      channel_impl_->ToSocket()->ToWebSocket()->ToWebSocketServer();
  server->AcceptLoop(accept_callback);
}

}  // namespace felicia