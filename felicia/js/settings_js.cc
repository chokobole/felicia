// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/js/settings_js.h"

#include <iostream>

#include "felicia/js/typed_call.h"

namespace felicia {
namespace communication {

constexpr const char* kPeriod = "period";
constexpr const char* kBufferSize = "bufferSize";
constexpr const char* kIsDynamicBuffer = "isDynamicBuffer";
constexpr const char* kQueueSize = "queueSize";

Napi::FunctionReference JsSettings::constructor_;

// static
void JsSettings::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(
      env, "Settings",
      {InstanceAccessor(kPeriod, &JsSettings::period, &JsSettings::set_period),
       InstanceAccessor(kBufferSize, &JsSettings::buffer_size,
                        &JsSettings::set_buffer_size),
       InstanceAccessor(kIsDynamicBuffer, &JsSettings::is_dynamic_buffer,
                        &JsSettings::set_is_dynamic_buffer),
       InstanceAccessor(kQueueSize, &JsSettings::queue_size,
                        &JsSettings::set_queue_size)});

  constructor_ = Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("Settings", func);
}

// static
Napi::Object JsSettings::New(Napi::Env env, const Settings& settings) {
  Napi::EscapableHandleScope scope(env);

  Napi::Object arg = Napi::Object::New(env);
  arg[kPeriod] = Napi::Number::New(env, settings.period.InMillisecondsF());
  arg[kBufferSize] = Napi::Number::New(env, settings.buffer_size.bytes());
  arg[kIsDynamicBuffer] = Napi::Boolean::New(env, settings.is_dynamic_buffer);
  arg[kQueueSize] = Napi::Number::New(env, settings.queue_size);

  Napi::Object object = constructor_.New({arg});

  return scope.Escape(napi_value(object)).ToObject();
}

JsSettings::JsSettings(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<JsSettings>(info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() == 0) {
    settings_ = Settings();
  } else if (info.Length() == 1) {
    Napi::Object settings_arg = info[0].As<Napi::Object>();
    Napi::Value period = settings_arg[kPeriod];
    if (!period.IsUndefined()) {
      set_period(info, period);
    }

    Napi::Value buffer_size = settings_arg[kBufferSize];
    if (!buffer_size.IsUndefined()) {
      set_buffer_size(info, buffer_size);
    }

    Napi::Value is_dynamic_buffer = settings_arg[kIsDynamicBuffer];
    if (!is_dynamic_buffer.IsUndefined()) {
      set_is_dynamic_buffer(info, is_dynamic_buffer);
    }

    Napi::Value queue_size = settings_arg[kQueueSize];
    if (!queue_size.IsUndefined()) {
      set_queue_size(info, queue_size);
    }
  } else {
    THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
    return;
  }
}

Napi::Value JsSettings::period(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), settings_.period.InMillisecondsF());
}

void JsSettings::set_period(const Napi::CallbackInfo& info,
                            const Napi::Value& value) {
  settings_.period = base::TimeDelta::FromMillisecondsD(
      value.As<Napi::Number>().DoubleValue());
}

Napi::Value JsSettings::buffer_size(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), settings_.buffer_size.bytes());
}

void JsSettings::set_buffer_size(const Napi::CallbackInfo& info,
                                 const Napi::Value& value) {
  settings_.buffer_size = Bytes::FromBytes(
      static_cast<size_t>(value.As<Napi::Number>().DoubleValue()));
}

Napi::Value JsSettings::is_dynamic_buffer(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), settings_.is_dynamic_buffer);
}

void JsSettings::set_is_dynamic_buffer(const Napi::CallbackInfo& info,
                                       const Napi::Value& value) {
  settings_.is_dynamic_buffer = value.As<Napi::Boolean>().Value();
}

Napi::Value JsSettings::queue_size(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), settings_.queue_size);
}

void JsSettings::set_queue_size(const Napi::CallbackInfo& info,
                                const Napi::Value& value) {
  settings_.queue_size =
      static_cast<uint8_t>(value.As<Napi::Number>().Uint32Value());
}

}  // namespace communication
}  // namespace felicia