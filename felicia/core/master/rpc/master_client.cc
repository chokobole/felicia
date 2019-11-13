// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/master/rpc/master_client.h"

namespace felicia {

MasterClient::MasterClient(std::shared_ptr<::grpc::Channel> channel)
    : Client<grpc::MasterService>(channel) {}

MasterClient::~MasterClient() = default;

Status MasterClient::Start() { return Run(); }

Status MasterClient::Stop() { return Shutdown(); }

#define MASTER_METHOD(Method, method, cancelable) \
  FEL_GRPC_CLIENT_METHOD_DEFINE(MasterClient, Method)
#include "felicia/core/master/rpc/master_method_list.h"
#undef MASTER_METHOD

}  // namespace felicia