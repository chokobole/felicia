// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/udp_channel.h"

#include "third_party/chromium/base/bind.h"

#include "felicia/core/channel/socket/udp_client_socket.h"
#include "felicia/core/channel/socket/udp_server_socket.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/message/message_io.h"

namespace felicia {

namespace {

Bytes kMaximumBufferSize = Bytes::FromKilloBytes(64);

}  // namespace

UDPChannel::UDPChannel() = default;

UDPChannel::~UDPChannel() = default;

bool UDPChannel::IsUDPChannel() const { return true; }

ChannelDef::Type UDPChannel::type() const {
  return ChannelDef::CHANNEL_TYPE_UDP;
}

bool UDPChannel::ShouldReceiveMessageWithHeader() const { return true; }

StatusOr<ChannelDef> UDPChannel::Bind() {
  DCHECK(!channel_impl_);
  channel_impl_ = std::make_unique<UDPServerSocket>();
  UDPServerSocket* server_socket =
      channel_impl_->ToSocket()->ToUDPSocket()->ToUDPServerSocket();
  return server_socket->Bind();
}

void UDPChannel::Connect(const ChannelDef& channel_def,
                         StatusOnceCallback callback) {
  DCHECK(!channel_impl_);
  DCHECK(!callback.is_null());
  net::AddressList addrlist;
  Status s = ToNetAddressList(channel_def, &addrlist);
  if (!s.ok()) {
    std::move(callback).Run(s);
    return;
  }
  channel_impl_ = std::make_unique<UDPClientSocket>();
  UDPClientSocket* client_socket =
      channel_impl_->ToSocket()->ToUDPSocket()->ToUDPClientSocket();
  client_socket->Connect(addrlist, std::move(callback));
}

void UDPChannel::SetSendBufferSize(Bytes bytes) {
  if (bytes > kMaximumBufferSize) {
    LOG(ERROR) << "UDP buffer can't exceed " << bytes;
    bytes = kMaximumBufferSize;
  }
  Channel::SetSendBufferSize(bytes);
}

void UDPChannel::SetReceiveBufferSize(Bytes bytes) {
  if (bytes > kMaximumBufferSize) {
    LOG(ERROR) << "UDP buffer can't exceed " << bytes;
    bytes = kMaximumBufferSize;
  }
  Channel::SetReceiveBufferSize(bytes);
}

bool UDPChannel::TrySetEnoughReceiveBufferSize(int capacity) {
  receive_buffer_.SetEnoughCapacityIfDynamic(kMaximumBufferSize);
  return true;
}

}  // namespace felicia