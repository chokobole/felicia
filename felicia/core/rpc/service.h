#ifndef FELIICA_CORE_RPC_SERVICE_H_
#define FELIICA_CORE_RPC_SERVICE_H_

namespace felicia {
namespace rpc {

template <typename T, typename SFINAE = void>
class Service;

}  // namespace rpc
}  // namespace felicia

#include "felicia/core/rpc/grpc_service_impl.h"
#include "felicia/core/rpc/ros_service_impl.h"

#endif  // FELIICA_CORE_RPC_SERVICE_H_