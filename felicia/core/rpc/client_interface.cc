#include "felicia/core/rpc/client_interface.h"

namespace felicia {
namespace rpc {

ClientInterface::ClientInterface() = default;
ClientInterface::~ClientInterface() = default;

void ClientInterface::set_service_info(const ServiceInfo& service_info) {
  service_info_ = service_info;
}

}  // namespace rpc
}  // namespace felicia