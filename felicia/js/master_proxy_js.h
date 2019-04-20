#ifndef FELICIA_JS_MASTER_PROXY_JS_H_
#define FELICIA_JS_MASTER_PROXY_JS_H_

#include "napi.h"
#include "uv.h"

#include "felicia/core/master/master_proxy.h"
#include "felicia/core/message/protobuf_loader.h"
#include "felicia/core/node/dynamic_subscribing_node.h"

namespace felicia {

class JsMasterProxy : public ::Napi::ObjectWrap<JsMasterProxy> {
 public:
  static void Init(::Napi::Env env, ::Napi::Object exports);
  JsMasterProxy(const ::Napi::CallbackInfo& info);

  static napi_env CurrentEnv();

  static void SetBackground(const ::Napi::CallbackInfo& info);

#if defined(FEL_WIN_NO_GRPC)
  static ::Napi::Value StartGrpcMasterClient(const ::Napi::CallbackInfo& info);
  static ::Napi::Value is_client_info_set(const ::Napi::CallbackInfo& info);
#endif

  static ::Napi::Value Start(const ::Napi::CallbackInfo& info);
  static ::Napi::Value Stop(const ::Napi::CallbackInfo& info);

  static void Run(const ::Napi::CallbackInfo& info);

  static void RequestRegisterDynamicSubscribingNode(
      const ::Napi::CallbackInfo& info);

 private:
  friend void OnCallback(uv_async_t*);

  static void OnNewMessage(const std::string& topic,
                           DynamicProtobufMessage&& message);

  static void OnSubscriptionError(const std::string& topic, const Status& s);

  static ::Napi::FunctionReference constructor_;

  static ::Napi::FunctionReference on_new_message_;
  static ::Napi::FunctionReference on_subscription_error_;
};

}  // namespace felicia

#endif  // FELICIA_JS_MASTER_PROXY_JS_H_
