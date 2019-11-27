// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_SERVICE_IMPL_H_
#define FELICIA_CORE_RPC_ROS_SERVICE_IMPL_H_

#include <ros/message_traits.h>
#include <ros/service_traits.h>

#include "felicia/core/lib/error/status.h"
#include "felicia/core/rpc/ros_util.h"

namespace felicia {
namespace rpc {

#define FEL_ROS_SERVICE Service<T, std::enable_if_t<IsRosService<T>::value>>

template <typename T>
class FEL_ROS_SERVICE {
 public:
  typedef T RosService;
  typedef typename RosService::Request Request;
  typedef typename RosService::Response Response;

  Service() = default;
  virtual ~Service() = default;

  std::string GetServiceTypeName() const {
    return ros::service_traits::DataType<RosService>::value();
  }
  std::string GetServiceMD5Sum() const {
    return ros::service_traits::MD5Sum<RosService>::value();
  }
  std::string GetRequestTypeName() const {
    return ros::message_traits::DataType<Request>::value();
  }
  std::string GetResponseTypeName() const {
    return ros::message_traits::DataType<Response>::value();
  }

  virtual void Handle(const Request* request, Response* response,
                      StatusOnceCallback callback) = 0;
};

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_CORE_RPC_ROS_SERVICE_IMPL_H_

#endif  // defined(HAS_ROS)