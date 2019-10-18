#ifndef FELICIA_CORE_COMMUNICATION_SERVICE_SERVER_H_
#define FELICIA_CORE_COMMUNICATION_SERVICE_SERVER_H_

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/compiler_specific.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/channel/ros_protocol.h"
#include "felicia/core/communication/register_state.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/core/rpc/grpc_util.h"
#include "felicia/core/rpc/ros_util.h"
#include "felicia/core/rpc/server.h"

namespace felicia {

template <typename ServiceTy>
class SimpleGrpcServer : public rpc::Server<ServiceTy> {
 public:
  // rpc::Server<ServiceTy> methods
  Status RegisterService(::grpc::ServerBuilder* builder) override {
    this->service_ = std::make_unique<ServiceTy>(builder);
    return Status::OK();
  }
};

namespace internal {

template <typename T, typename SFINAE = void>
struct ServiceServerTraits;

template <typename T>
struct ServiceServerTraits<
    T, std::enable_if_t<IsGrpcService<typename T::GrpcService>::value>> {
  typedef SimpleGrpcServer<T> ServerTy;
};

#if defined(HAS_ROS)
template <typename T>
struct ServiceServerTraits<
    T, std::enable_if_t<IsRosService<typename T::RosService>::value>> {
  typedef rpc::Server<T> ServerTy;
};
#endif  // defined(HAS_ROS)

}  // namespace internal

template <typename ServiceTy,
          typename ServerTy =
              typename internal::ServiceServerTraits<ServiceTy>::ServerTy>
class ServiceServer {
 public:
  ServiceServer() = default;

  ALWAYS_INLINE bool IsRegistering() const {
    return register_state_.IsRegistering();
  }
  ALWAYS_INLINE bool IsRegistered() const {
    return register_state_.IsRegistered();
  }
  ALWAYS_INLINE bool IsUnregistering() const {
    return register_state_.IsUnregistering();
  }
  ALWAYS_INLINE bool IsUnregistered() const {
    return register_state_.IsUnregistered();
  }

  void RequestRegister(const NodeInfo& node_info, const std::string& service,
                       StatusOnceCallback callback = StatusOnceCallback());

  void RequestUnregister(const NodeInfo& node_info, const std::string& service,
                         StatusOnceCallback callback = StatusOnceCallback());

 protected:
  void OnRegisterServiceServerAsync(const RegisterServiceServerRequest* request,
                                    RegisterServiceServerResponse* response,
                                    StatusOnceCallback callback,
                                    const Status& s);

  void OnUnegisterServiceServerAsync(
      const UnregisterServiceServerRequest* request,
      UnregisterServiceServerResponse* response, StatusOnceCallback callback,
      const Status& s);

  ServerTy server_;
  communication::RegisterState register_state_;
};

template <typename ServiceTy, typename ServerTy>
void ServiceServer<ServiceTy, ServerTy>::RequestRegister(
    const NodeInfo& node_info, const std::string& service,
    StatusOnceCallback callback) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE,
        base::BindOnce(&ServiceServer<ServiceTy, ServerTy>::RequestRegister,
                       base::Unretained(this), node_info, service,
                       std::move(callback)));
    return;
  }

  if (!IsUnregistered()) {
    internal::LogOrCallback(std::move(callback),
                            register_state_.InvalidStateError());
    return;
  }

  register_state_.ToRegistering(FROM_HERE);

  server_.Start();
  server_.set_use_ros_channel(IsUsingRosProtocol(service));
  server_.Run();

  RegisterServiceServerRequest* request = new RegisterServiceServerRequest();
  *request->mutable_node_info() = node_info;
  ServiceInfo* service_info = request->mutable_service_info();
  service_info->set_service(service);
  service_info->set_type_name(server_.service_name());
  ChannelSource* channel_source = service_info->mutable_service_source();
  *channel_source->add_channel_defs() = server_.channel_def();
  RegisterServiceServerResponse* response = new RegisterServiceServerResponse();

  master_proxy.RegisterServiceServerAsync(
      request, response,
      base::BindOnce(
          &ServiceServer<ServiceTy, ServerTy>::OnRegisterServiceServerAsync,
          base::Unretained(this), base::Owned(request), base::Owned(response),
          std::move(callback)));
}

template <typename ServiceTy, typename ServerTy>
void ServiceServer<ServiceTy, ServerTy>::RequestUnregister(
    const NodeInfo& node_info, const std::string& service,
    StatusOnceCallback callback) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE,
        base::BindOnce(&ServiceServer<ServiceTy, ServerTy>::RequestUnregister,
                       base::Unretained(this), node_info, service,
                       std::move(callback)));
    return;
  }

  if (!IsRegistered()) {
    internal::LogOrCallback(std::move(callback),
                            register_state_.InvalidStateError());
    return;
  }

  register_state_.ToUnregistering(FROM_HERE);

  UnregisterServiceServerRequest* request =
      new UnregisterServiceServerRequest();
  *request->mutable_node_info() = node_info;
  request->set_service(service);
  UnregisterServiceServerResponse* response =
      new UnregisterServiceServerResponse();

  master_proxy.UnregisterServiceServerAsync(
      request, response,
      base::BindOnce(
          &ServiceServer<ServiceTy, ServerTy>::OnUnegisterServiceServerAsync,
          base::Unretained(this), base::Owned(request), base::Owned(response),
          std::move(callback)));
}

template <typename ServiceTy, typename ServerTy>
void ServiceServer<ServiceTy, ServerTy>::OnRegisterServiceServerAsync(
    const RegisterServiceServerRequest* request,
    RegisterServiceServerResponse* response, StatusOnceCallback callback,
    const Status& s) {
  if (!IsRegistering()) {
    internal::LogOrCallback(std::move(callback),
                            register_state_.InvalidStateError());
    return;
  }

  if (!s.ok()) {
    register_state_.ToUnregistered(FROM_HERE);
    internal::LogOrCallback(std::move(callback), s);
    return;
  }

  register_state_.ToRegistered(FROM_HERE);
  internal::LogOrCallback(std::move(callback), s);
}

template <typename ServiceTy, typename ServerTy>
void ServiceServer<ServiceTy, ServerTy>::OnUnegisterServiceServerAsync(
    const UnregisterServiceServerRequest* request,
    UnregisterServiceServerResponse* response, StatusOnceCallback callback,
    const Status& s) {
  if (!IsUnregistering()) {
    internal::LogOrCallback(std::move(callback),
                            register_state_.InvalidStateError());
    return;
  }

  if (!s.ok()) {
    register_state_.ToRegistered(FROM_HERE);
    internal::LogOrCallback(std::move(callback), s);
    return;
  }

  register_state_.ToUnregistered(FROM_HERE);
  server_.Shutdown();

  internal::LogOrCallback(std::move(callback), s);
}

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_SERVICE_SERVER_H_