#ifndef FELICIA_JS_MASTER_PROXY_JS_H_
#define FELICIA_JS_MASTER_PROXY_JS_H_

#include "napi.h"

#include "felicia/core/master/master_proxy.h"

namespace felicia {

class JsMasterProxy : public ::Napi::ObjectWrap<JsMasterProxy> {
 public:
  static void Init(::Napi::Env env, ::Napi::Object exports);
  JsMasterProxy(const ::Napi::CallbackInfo& info);

  static void SetBackground(const ::Napi::CallbackInfo& info);

  static ::Napi::Value Start(const ::Napi::CallbackInfo& info);
  static ::Napi::Value Stop(const ::Napi::CallbackInfo& info);

  static void Run(const ::Napi::CallbackInfo& info);

  static void RequestRegisterNode(const ::Napi::CallbackInfo& info);

 private:
  static ::Napi::FunctionReference constructor_;
};

}  // namespace felicia

#endif  // FELICIA_JS_MASTER_PROXY_JS_H_
