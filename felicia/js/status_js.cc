// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/js/status_js.h"

#include "felicia/js/typed_call.h"

namespace felicia {

Napi::FunctionReference JsStatus::constructor_;

// static
void JsStatus::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func =
      DefineClass(env, "Status",
                  {
                      StaticMethod("OK", &JsStatus::OK),
                      InstanceMethod("errorCode", &JsStatus::error_code),
                      InstanceMethod("errorMessage", &JsStatus::error_message),
                      InstanceMethod("ok", &JsStatus::ok),
                  });

  constructor_ = Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("Status", func);
}

// static
Napi::Object JsStatus::New(Napi::Env env, Status s) {
  Napi::EscapableHandleScope scope(env);

  Napi::Object object = constructor_.New(
      {Napi::Number::New(env, static_cast<double>(s.error_code())),
       Napi::String::New(env, std::move(s).error_message())});

  return scope.Escape(napi_value(object)).ToObject();
}

JsStatus::JsStatus(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<JsStatus>(info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() == 0) {
    status_ = Status();
  } else if (info.Length() == 2) {
    felicia::error::Code error_code = static_cast<felicia::error::Code>(
        info[0].As<Napi::Number>().Int32Value());
    std::string error_message = info[1].As<Napi::String>().Utf8Value();

    status_ = Status(error_code, error_message);
  } else {
    THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
    return;
  }
}

// static
Napi::Value JsStatus::OK(const Napi::CallbackInfo& info) {
  return constructor_.New({});
}

Napi::Value JsStatus::error_code(const Napi::CallbackInfo& info) {
  return TypedCall(info, &Status::error_code, &status_);
}

Napi::Value JsStatus::error_message(const Napi::CallbackInfo& info) {
  return TypedCall(
      info, (const std::string& (Status::*)() const&)&Status::error_message,
      &status_);
}

Napi::Value JsStatus::ok(const Napi::CallbackInfo& info) {
  return TypedCall(info, &Status::ok, &status_);
}

}  // namespace felicia