#ifndef FELICIA_CORE_RPC_GRPC_UTIL_H_
#define FELICIA_CORE_RPC_GRPC_UTIL_H_

#include "grpcpp/grpcpp.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

inline Status FromGrpcStatus(const ::grpc::Status& s) {
  if (s.ok()) {
    return Status::OK();
  } else {
    return Status(static_cast<felicia::error::Code>(s.error_code()),
                  s.error_message());
  }
}

inline ::grpc::Status ToGrpcStatus(const Status& s) {
  if (s.ok()) {
    return ::grpc::Status::OK;
  } else {
    if (static_cast<int>(s.error_code()) >=
        static_cast<int>(
            error::Code::
                DO_NOT_USE_RESERVED_FOR_FUTURE_EXPANSION_USE_DEFAULT_IN_SWITCH_INSTEAD_)) {
      return ::grpc::Status(::grpc::StatusCode::UNKNOWN, s.error_message());
    } else {
      return ::grpc::Status(static_cast<::grpc::StatusCode>(s.error_code()),
                            s.error_message());
    }
  }
}

EXPORT std::shared_ptr<::grpc::Channel> ConnectToGrpcServer(
    const std::string& ip, uint16_t port);

}  // namespace felicia

#endif  // FELICIA_CORE_RPC_GRPC_UTIL_H_