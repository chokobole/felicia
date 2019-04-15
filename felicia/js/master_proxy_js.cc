#include "felicia/js/master_proxy_js.h"

#include "felicia/js/status_js.h"
#include "felicia/js/typed_call.h"

namespace felicia {

::Napi::FunctionReference JsMasterProxy::constructor_;

// static
void JsMasterProxy::Init(::Napi::Env env, ::Napi::Object exports) {
  ::Napi::HandleScope scope(env);

  ::Napi::Function func =
      DefineClass(env, "MasterProxy",
                  {
                      StaticMethod("start", &JsMasterProxy::Start),
                      StaticMethod("stop", &JsMasterProxy::Stop),
                      StaticMethod("run", &JsMasterProxy::Run),
                      StaticMethod("requestRegisterNode",
                                   &JsMasterProxy::RequestRegisterNode),
                  });

  constructor_ = ::Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("MasterProxy", func);
}

JsMasterProxy::JsMasterProxy(const ::Napi::CallbackInfo& info)
    : ::Napi::ObjectWrap<JsMasterProxy>(info) {
  ::Napi::Env env = info.Env();
  ::Napi::HandleScope scope(env);

  ::Napi::TypeError::New(env, "Cann't instantiate MasterProxy.")
      .ThrowAsJavaScriptException();
}

// static
::Napi::Value JsMasterProxy::Start(const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  ::Napi::EscapableHandleScope scope(env);

  JS_CHECK_NUM_ARGS(info.Env(), 0);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  Status s = master_proxy.Start();

  ::Napi::Object obj = JsStatus::New(s, info);

  return scope.Escape(napi_value(obj)).ToObject();
}

// static
::Napi::Value JsMasterProxy::Stop(const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  ::Napi::EscapableHandleScope scope(env);

  JS_CHECK_NUM_ARGS(info.Env(), 0);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  Status s = master_proxy.Start();

  ::Napi::Object obj = JsStatus::New(s, info);

  return scope.Escape(napi_value(obj)).ToObject();
}

// static
void JsMasterProxy::Run(const ::Napi::CallbackInfo& info) {
  JS_CHECK_NUM_ARGS(info.Env(), 0);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.Run();
}

// static
void JsMasterProxy::RequestRegisterNode(const ::Napi::CallbackInfo& info) {
  NOTIMPLEMENTED();
}

}  // namespace felicia