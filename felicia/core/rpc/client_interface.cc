// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/rpc/client_interface.h"

namespace felicia {
namespace rpc {

ClientInterface::ClientInterface() = default;
ClientInterface::~ClientInterface() = default;

void ClientInterface::set_service_info(const ServiceInfo& service_info) {
  service_info_ = service_info;
}

}  // namespace rpc
}  // namespace felicia