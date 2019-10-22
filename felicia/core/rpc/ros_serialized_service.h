#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_SERIALZIED_SERVICE_H_
#define FELICIA_CORE_RPC_ROS_SERIALZIED_SERVICE_H_

#include <ros/service_traits.h>

#include "felicia/core/message/serialized_message.h"

namespace felicia {
namespace rpc {

struct RosSerializedService {
  typedef SerializedMessage Request;
  typedef SerializedMessage Response;
  Request request;
  Response response;
};

}  // namespace rpc
}  // namespace felicia

// These are just for compilation due to
// felicia::rpc::Cient<felicia::rpc::RosSerializedService>, never used.
namespace ros {
namespace service_traits {

template <>
struct MD5Sum<::felicia::rpc::RosSerializedService> {
  static const char* value() { return ""; }
};

template <>
struct DataType<::felicia::rpc::RosSerializedService> {
  static const char* value() { return ""; }
};

}  // namespace service_traits
}  // namespace ros

#endif  // FELICIA_CORE_RPC_ROS_SERIALZIED_SERVICE_H_

#endif  // defined(HAS_ROS)