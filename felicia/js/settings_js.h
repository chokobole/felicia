// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_JS_SETTINGS_JS_H_
#define FELICIA_JS_SETTINGS_JS_H_

#include "napi.h"

#include "felicia/core/communication/settings.h"
#include "felicia/js/type_convertor_forward.h"

namespace felicia {
namespace communication {

class JsSettings : public Napi::ObjectWrap<JsSettings> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  static Napi::Object New(Napi::Env env, const Settings& settings);
  JsSettings(const Napi::CallbackInfo& info);

  Napi::Value period(const Napi::CallbackInfo& info);
  void set_period(const Napi::CallbackInfo& info, const Napi::Value& value);
  Napi::Value buffer_size(const Napi::CallbackInfo& info);
  void set_buffer_size(const Napi::CallbackInfo& info,
                       const Napi::Value& value);
  Napi::Value is_dynamic_buffer(const Napi::CallbackInfo& info);
  void set_is_dynamic_buffer(const Napi::CallbackInfo& info,
                             const Napi::Value& value);
  Napi::Value queue_size(const Napi::CallbackInfo& info);
  void set_queue_size(const Napi::CallbackInfo& info, const Napi::Value& value);

  const Settings& settings() const { return settings_; }

 private:
  static Napi::FunctionReference constructor_;

  Settings settings_;
};

}  // namespace communication

namespace js {

template <>
class TypeConvertor<communication::Settings> {
 public:
  static const communication::Settings& ToNativeValue(Napi::Value value) {
    return Napi::ObjectWrap<communication::JsSettings>::Unwrap(
               value.As<Napi::Object>())
        ->settings();
  }

  static Napi::Value ToJSValue(Napi::Env env,
                               const communication::Settings& value) {
    return communication::JsSettings::New(env, value);
  }
};

}  // namespace js

}  // namespace felicia

#endif  // FELICIA_JS_SETTINGS_JS_H_