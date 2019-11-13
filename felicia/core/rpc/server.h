// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELIICA_CORE_RPC_SERVER_H_
#define FELIICA_CORE_RPC_SERVER_H_

namespace felicia {
namespace rpc {

template <typename T, typename SFINAE = void>
class Server;

}  // namespace rpc
}  // namespace felicia

#include "felicia/core/rpc/grpc_server_impl.h"
#include "felicia/core/rpc/ros_server_impl.h"

#endif  // FELIICA_CORE_RPC_SERVER_H_