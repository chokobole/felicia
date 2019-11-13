// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#include "felicia/core/rpc/ros_serialized_service_interface.h"

namespace felicia {
namespace rpc {

RosSerializedServiceInterface::RosSerializedServiceInterface() = default;

RosSerializedServiceInterface::~RosSerializedServiceInterface() = default;

std::string RosSerializedServiceInterface::GetServiceTypeName() const {
  return service_type_name_;
}

std::string RosSerializedServiceInterface::GetServiceMD5Sum() const {
  return service_md5sum_;
}

std::string RosSerializedServiceInterface::GetRequestTypeName() const {
  return request_type_name_;
}

std::string RosSerializedServiceInterface::GetResponseTypeName() const {
  return response_type_name_;
}

}  // namespace rpc
}  // namespace felicia

#endif  // defined(HAS_ROS)