#ifndef FELICIA_CORE_RPC_GRPC_UTIL_H_
#define FELICIA_CORE_RPC_GRPC_UTIL_H_

#include "grpcpp/grpcpp.h"
#include "third_party/chromium/base/template_util.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

template <typename, typename = void>
struct IsGrpcService : std::false_type {};

template <typename T>
struct IsGrpcService<
    T, base::void_t<decltype(T::service_full_name()),
                    decltype(std::declval<typename T::Stub>()),
                    decltype(std::declval<typename T::Service>()),
                    decltype(std::declval<typename T::AsyncService>())>>
    : std::true_type {};

template <typename, typename = void>
struct IsGrpcServiceWrapper : std::false_type {};

template <typename T>
struct IsGrpcServiceWrapper<
    T, std::enable_if_t<IsGrpcService<typename T::GrpcService>::value>>
    : std::true_type {};

inline Status FromGrpcStatus(const ::grpc::Status& s) {
  if (s.ok()) {
    return Status::OK();
  } else {
    return Status(static_cast<felicia::error::Code>(s.error_code()),
                  s.error_message());
  }
}

inline ::grpc::Status ToGrpcStatus(Status s) {
  if (s.ok()) {
    return ::grpc::Status::OK;
  } else {
    if (static_cast<int>(s.error_code()) >=
        static_cast<int>(
            error::Code::
                DO_NOT_USE_RESERVED_FOR_FUTURE_EXPANSION_USE_DEFAULT_IN_SWITCH_INSTEAD_)) {
      return ::grpc::Status(::grpc::StatusCode::UNKNOWN,
                            std::move(s).error_message());
    } else {
      return ::grpc::Status(static_cast<::grpc::StatusCode>(s.error_code()),
                            std::move(s).error_message());
    }
  }
}

FEL_EXPORT std::shared_ptr<::grpc::Channel> ConnectToGrpcServer(
    const std::string& ip, uint16_t port);

}  // namespace felicia

#endif  // FELICIA_CORE_RPC_GRPC_UTIL_H_