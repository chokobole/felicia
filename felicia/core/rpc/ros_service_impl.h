#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_SERVICE_IMPL_H_
#define FELICIA_CORE_RPC_ROS_SERVICE_IMPL_H_

#include <ros/service_traits.h>

#include "third_party/chromium/base/memory/ref_counted.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/rpc/ros_util.h"

namespace felicia {
namespace rpc {

#define FEL_ROS_SERVICE Service<T, std::enable_if_t<IsRosService<T>::value>>

template <typename T>
class FEL_ROS_SERVICE : public base::RefCountedThreadSafe<FEL_ROS_SERVICE> {
 public:
  typedef T RosService;
  typedef typename RosService::Request Request;
  typedef typename RosService::Response Response;

  static std::string DataType() {
    return ros::service_traits::DataType<RosService>::value();
  }
  static std::string MD5Sum() {
    return ros::service_traits::MD5Sum<RosService>::value();
  }

  Service() = default;

  virtual void Handle(const Request* request, Response* response,
                      StatusOnceCallback callback) = 0;

 protected:
  friend class base::RefCountedThreadSafe<FEL_ROS_SERVICE>;
  virtual ~Service() = default;
};

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_CORE_RPC_ROS_SERVICE_IMPL_H_

#endif  // defined(HAS_ROS)