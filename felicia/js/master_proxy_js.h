#ifndef FELICIA_JS_MASTER_PROXY_JS_H_
#define FELICIA_JS_MASTER_PROXY_JS_H_

#include "napi.h"
#include "uv.h"

#include "felicia/core/master/master_proxy.h"
#include "felicia/core/message/protobuf_loader.h"

namespace felicia {

class JsMasterProxy : public Napi::ObjectWrap<JsMasterProxy> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  JsMasterProxy(const Napi::CallbackInfo& info);

  static napi_env CurrentEnv();

  static void SetBackground(const Napi::CallbackInfo& info);

#if defined(FEL_WIN_NO_GRPC)
  static Napi::Value StartMasterClient(const Napi::CallbackInfo& info);
  static Napi::Value is_client_info_set(const Napi::CallbackInfo& info);
#endif

  static Napi::Value Start(const Napi::CallbackInfo& info);
  static Napi::Value Stop(const Napi::CallbackInfo& info);

  static void Run(const Napi::CallbackInfo& info);

  static void RequestRegisterTopicInfoWatcherNode(
      const Napi::CallbackInfo& info);

 private:
  static Napi::FunctionReference constructor_;
};

}  // namespace felicia

#endif  // FELICIA_JS_MASTER_PROXY_JS_H_
