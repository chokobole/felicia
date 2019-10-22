#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_UTIL_H_
#define FELICIA_CORE_RPC_ROS_UTIL_H_

#include <type_traits>

#include <ros/message_traits.h>

namespace felicia {

template <typename, typename = void>
struct IsRosService : std::false_type {};

template <typename T>
struct IsRosService<
    T, std::enable_if_t<
           ros::message_traits::IsMessage<typename T::Request>::value &&
           ros::message_traits::IsMessage<typename T::Response>::value>>
    : std::true_type {};

template <typename, typename = void>
struct IsRosServiceWrapper : std::false_type {};

template <typename T>
struct IsRosServiceWrapper<
    T, std::enable_if_t<IsRosService<typename T::RosService>::value>>
    : std::true_type {};

}  // namespace felicia

#endif  // FELICIA_CORE_RPC_ROS_UTIL_H_

#endif  // defined(HAS_ROS)