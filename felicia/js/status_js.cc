#include "felicia/js/status_js.h"

#include "third_party/chromium/base/logging.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

::Napi::FunctionReference JsStatus::constructor_;

// static
void JsStatus::Init(::Napi::Env env, ::Napi::Object exports) {
  ::Napi::HandleScope scope(env);

  ::Napi::Function func =
      DefineClass(env, "Status",
                  {
                      StaticMethod("OK", &JsStatus::OK),
                      InstanceMethod("error_code", &JsStatus::error_code),
                      InstanceMethod("error_message", &JsStatus::error_message),
                      InstanceMethod("ok", &JsStatus::ok),
                  });

  constructor_ = ::Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("Status", func);
}

// static
::Napi::Object JsStatus::New() { return constructor_.New({}); }

::Napi::Object JsStatus::New(::Napi::Value arg, ::Napi::Value arg2) {
  return constructor_.New({arg, arg2});
}

JsStatus::JsStatus(const ::Napi::CallbackInfo& info)
    : ::Napi::ObjectWrap<JsStatus>(info) {
  ::Napi::Env env = info.Env();
  ::Napi::HandleScope scope(env);

  if (info.Length() == 0) {
    status_ = Status();
  } else if (info.Length() == 2) {
    ::Napi::TypeError::New(env, "Not Implemented.")
        .ThrowAsJavaScriptException();
  } else {
    ::Napi::TypeError::New(env, "Wrong number of arguments.")
        .ThrowAsJavaScriptException();
  }
}

// static
::Napi::Value JsStatus::OK(const ::Napi::CallbackInfo& info) {
  return constructor_.New({});
}

::Napi::Value JsStatus::error_code(const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  return ::Napi::Number::New(env, status_.error_code());
}

::Napi::Value JsStatus::error_message(const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  return ::Napi::String::New(env, status_.error_message());
}

::Napi::Value JsStatus::ok(const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  return ::Napi::Boolean::New(env, status_.ok());
}

}  // namespace felicia