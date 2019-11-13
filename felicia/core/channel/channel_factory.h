// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_CHANNEL_FACTORY_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_FACTORY_H_

#include <memory>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/build/build_config.h"

#include "felicia/core/channel/tcp_channel.h"
#include "felicia/core/channel/udp_channel.h"
#include "felicia/core/channel/ws_channel.h"
#include "felicia/core/lib/base/export.h"
#if defined(OS_POSIX)
#include "felicia/core/channel/uds_channel.h"
#endif
#include "felicia/core/channel/settings.h"
#include "felicia/core/channel/shm_channel.h"

namespace felicia {

class FEL_EXPORT ChannelFactory {
 public:
  static std::unique_ptr<Channel> NewChannel(
      ChannelDef::Type channel_type,
      const channel::Settings& settings = channel::Settings());

  DISALLOW_COPY_AND_ASSIGN(ChannelFactory);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_FACTORY_H_