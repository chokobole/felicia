#ifndef FELIICA_CORE_RPC_SERVER_INTERFACE_H_
#define FELIICA_CORE_RPC_SERVER_INTERFACE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace felicia {
namespace rpc {

class EXPORT ServerInterface {
 public:
  ServerInterface();
  virtual ~ServerInterface();

  void set_service_info(const ServiceInfo& service_info);

  virtual Status Start() = 0;
  virtual Status Run() = 0;
  virtual Status Shutdown() = 0;

  virtual std::string GetServiceTypeName() const = 0;

  ChannelDef channel_def() const;

  // You need to set |port_| to server port here.
  virtual std::string ConfigureServerAddress();

 protected:
  ServiceInfo service_info_;
  uint16_t port_;
};

}  // namespace rpc
}  // namespace felicia

#endif  // FELIICA_CORE_RPC_SERVER_INTERFACE_H_