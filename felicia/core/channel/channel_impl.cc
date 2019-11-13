// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/channel_impl.h"

#include "third_party/chromium/base/logging.h"

namespace felicia {

ChannelImpl::~ChannelImpl() = default;

bool ChannelImpl::IsSocket() const { return false; }
bool ChannelImpl::IsSharedMemory() const { return false; }

Socket* ChannelImpl::ToSocket() {
  DCHECK(IsSocket());
  return reinterpret_cast<Socket*>(this);
}

SharedMemory* ChannelImpl::ToSharedMemory() {
  DCHECK(IsSharedMemory());
  return reinterpret_cast<SharedMemory*>(this);
}

ChannelDef ToChannelDef(const net::IPEndPoint& ip_endpoint,
                        ChannelDef::Type type) {
  DCHECK(type == ChannelDef::CHANNEL_TYPE_TCP ||
         type == ChannelDef::CHANNEL_TYPE_UDP ||
         type == ChannelDef::CHANNEL_TYPE_WS);
  ChannelDef channel_def;
  IPEndPoint* endpoint = channel_def.mutable_ip_endpoint();
  endpoint->set_ip(ip_endpoint.address().ToString());
  endpoint->set_port(ip_endpoint.port());
  channel_def.set_type(type);
  return channel_def;
}

}  // namespace felicia