// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_SERVICE_MANAGER_H_
#define FELICIA_CORE_RPC_ROS_SERVICE_MANAGER_H_

#include "third_party/chromium/base/memory/ref_counted.h"

#include "felicia/core/rpc/ros_service_handler.h"

namespace felicia {
namespace rpc {
namespace internal {

template <typename Service, typename Request, typename Response>
class RosServiceManager
    : public base::RefCounted<RosServiceManager<Service, Request, Response>> {
 public:
  typedef RosServiceHandler<Service, Request, Response> Handler;
  typedef RosServiceManager<Service, Request, Response> Manager;

  RosServiceManager() : service_(std::make_unique<Service>()) {}
  explicit RosServiceManager(std::unique_ptr<Service> service)
      : service_(std::move(service)) {}

  void AddHandler(std::unique_ptr<Handler> handler) {
    DFAKE_SCOPED_LOCK(add_remove_);
    handlers_.push_back(std::move(handler));
  }

  void RemoveHandler(Handler* handler) {
    DFAKE_SCOPED_LOCK(add_remove_);
    auto it = std::find_if(handlers_.begin(), handlers_.end(),
                           [handler](std::unique_ptr<Handler>& elem) {
                             return elem.get() == handler;
                           });
    handlers_.erase(it, handlers_.end());
  }

  void ClearHandlers() {
    DFAKE_SCOPED_LOCK(add_remove_);
    handlers_.clear();
  }

  Service* service() { return service_.get(); }

 private:
  friend class base::RefCounted<RosServiceManager>;
  virtual ~RosServiceManager() = default;

  std::unique_ptr<Service> service_;
  std::vector<std::unique_ptr<Handler>> handlers_;
  DFAKE_MUTEX(add_remove_);
};

}  // namespace internal
}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_CORE_RPC_ROS_SERVICE_MANAGER_H_

#endif  // defined(HAS_ROS)
