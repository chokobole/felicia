// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/master/rpc/master_service.h"

namespace felicia {

MasterService::MasterService(Master* master, ::grpc::ServerBuilder* builder)
    : Service<grpc::MasterService>(builder), master_(master) {}

void MasterService::EnqueueRequests() {
#define MASTER_METHOD(Method, method, cancelable) \
  FEL_ENQUEUE_REQUEST(MasterService, Method, cancelable);
#include "felicia/core/master/rpc/master_method_list.h"
#undef MASTER_METHOD
}

#define MASTER_METHOD(Method, method, cancelable) \
  FEL_GRPC_SERVICE_METHOD_DEFINE(MasterService, master_, Method, cancelable);
#include "felicia/core/master/rpc/master_method_list.h"
#undef MASTER_METHOD

}  // namespace felicia