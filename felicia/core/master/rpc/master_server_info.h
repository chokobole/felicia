// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MASTER_RPC_MASTER_SERVER_INFO_H_
#define FELICIA_CORE_MASTER_RPC_MASTER_SERVER_INFO_H_

#include <stdint.h>

#include "third_party/chromium/net/base/ip_address.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

FEL_EXPORT net::IPAddress ResolveMasterServerIp();

FEL_EXPORT uint16_t ResolveMasterServerPort();

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_RPC_MASTER_SERVER_INFO_H_