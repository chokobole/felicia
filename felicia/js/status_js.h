#ifndef FELICIA_JS_STATUS_JS_H_
#define FELICIA_JS_STATUS_JS_H_

#include "napi.h"

#include "felicia/core/lib/error/status.h"

namespace felicia {

class JsStatus : public ::Napi::ObjectWrap<JsStatus> {
 public:
  static void Init(::Napi::Env env, ::Napi::Object exports);
  static ::Napi::Object New(const Status& s, const ::Napi::CallbackInfo& info);
  JsStatus(const ::Napi::CallbackInfo& info);

  static ::Napi::Value OK(const ::Napi::CallbackInfo& info);

  ::Napi::Value error_code(const ::Napi::CallbackInfo& info);
  ::Napi::Value error_message(const ::Napi::CallbackInfo& info);
  ::Napi::Value ok(const ::Napi::CallbackInfo& info);

 private:
  static ::Napi::FunctionReference constructor_;

  Status status_;
};

}  // namespace felicia

#endif  // FELICIA_JS_STATUS_JS_H_