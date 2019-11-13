#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_SERIALZIED_SERVICE_INTERFACE_H_
#define FELICIA_CORE_RPC_ROS_SERIALZIED_SERVICE_INTERFACE_H_

#include "third_party/chromium/base/memory/ref_counted.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/message/serialized_message.h"
#include "felicia/core/rpc/ros_util.h"
#include "felicia/core/rpc/service.h"

namespace felicia {
namespace rpc {

class FEL_EXPORT RosSerializedServiceInterface
    : public base::RefCountedThreadSafe<RosSerializedServiceInterface> {
 public:
  typedef SerializedMessage Request;
  typedef SerializedMessage Response;

  RosSerializedServiceInterface();

  std::string GetServiceTypeName() const;
  std::string GetServiceMD5Sum() const;
  std::string GetRequestTypeName() const;
  std::string GetResponseTypeName() const;

  virtual void Handle(const SerializedMessage* request,
                      SerializedMessage* response,
                      StatusOnceCallback callback) = 0;

 protected:
  friend class base::RefCountedThreadSafe<RosSerializedServiceInterface>;
  virtual ~RosSerializedServiceInterface();

  std::string service_type_name_;
  std::string service_md5sum_;
  std::string request_type_name_;
  std::string response_type_name_;
};

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_CORE_RPC_ROS_SERIALZIED_SERVICE_INTERFACE_H_

#endif  // defined(HAS_ROS)