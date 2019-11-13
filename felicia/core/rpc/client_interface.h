// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELIICA_CORE_RPC_CLIENT_INTERFACE_H_
#define FELIICA_CORE_RPC_CLIENT_INTERFACE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace felicia {
namespace rpc {

class FEL_EXPORT ClientInterface {
 public:
  ClientInterface();
  virtual ~ClientInterface();

  void set_service_info(const ServiceInfo& service_info);

  virtual void Connect(const IPEndPoint& ip_endpoint,
                       StatusOnceCallback callback) = 0;
  virtual Status Run() = 0;
  virtual Status Shutdown() = 0;

 protected:
  ServiceInfo service_info_;
};

}  // namespace rpc
}  // namespace felicia

#endif  // FELIICA_CORE_RPC_CLIENT_INTERFACE_H_