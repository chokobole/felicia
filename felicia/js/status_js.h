#ifndef FELICIA_JS_STATUS_JS_H_
#define FELICIA_JS_STATUS_JS_H_

#include "napi.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/js/type_convertor_forward.h"

namespace felicia {

class JsStatus : public ::Napi::ObjectWrap<JsStatus> {
 public:
  static void Init(::Napi::Env env, ::Napi::Object exports);
  static ::Napi::Object New(::Napi::Env env, const Status& s);
  JsStatus(const ::Napi::CallbackInfo& info);

  static ::Napi::Value OK(const ::Napi::CallbackInfo& info);

  ::Napi::Value error_code(const ::Napi::CallbackInfo& info);
  ::Napi::Value error_message(const ::Napi::CallbackInfo& info);
  ::Napi::Value ok(const ::Napi::CallbackInfo& info);

  const Status& status() const { return status_; }

 private:
  static ::Napi::FunctionReference constructor_;

  Status status_;
};

namespace js {

template <>
class TypeConvertor<Status> {
 public:
  static Status ToNativeValue(::Napi::Value value) {
    return ::Napi::ObjectWrap<JsStatus>::Unwrap(value.As<::Napi::Object>())
        ->status();
  }

  static ::Napi::Value ToJSValue(::Napi::Env env, const Status& value) {
    return JsStatus::New(env, value);
  }
};

}  // namespace js

}  // namespace felicia

#endif  // FELICIA_JS_STATUS_JS_H_