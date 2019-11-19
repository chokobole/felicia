// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_SOCKET_HOST_RESOLVER_H_
#define FELICIA_CORE_CHANNEL_SOCKET_HOST_RESOLVER_H_

#include "third_party/chromium/net/base/address_family.h"
#include "third_party/chromium/net/base/address_list.h"

namespace felicia {

class HostResolver {
 public:
  static int ResolveHost(const std::string& host,
                         net::AddressFamily address_family,
                         net::HostResolverFlags host_resolver_flags,
                         net::AddressList* addrlist, int* os_error);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_HOST_RESOLVER_H_