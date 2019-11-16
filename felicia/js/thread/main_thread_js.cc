// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/js/thread/main_thread_js.h"

#include "felicia/core/thread/main_thread.h"
#include "felicia/js/type_conversion/typed_call.h"

namespace felicia {

Napi::FunctionReference JsMainThread::constructor_;

// static
void JsMainThread::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(
      env, "MainThread",
      {
          InstanceMethod("run", &JsMainThread::Run),
          InstanceMethod("runBackground", &JsMainThread::RunBackground),
          InstanceMethod("stop", &JsMainThread::Stop),
      });

  constructor_ = Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("mainThread", constructor_.New({}));
}

JsMainThread::JsMainThread(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<JsMainThread>(info) {}

void JsMainThread::Run(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  JS_CHECK_NUM_ARGS(env, 0);

  MainThread& main_thread = MainThread::GetInstance();
  main_thread.Run();
}

void JsMainThread::RunBackground(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  JS_CHECK_NUM_ARGS(env, 0);

  MainThread::SetBackground();
  MainThread& main_thread = MainThread::GetInstance();
  main_thread.RunBackground();
}

void JsMainThread::Stop(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  JS_CHECK_NUM_ARGS(env, 0);

  MainThread& main_thread = MainThread::GetInstance();
  main_thread.Stop();
}

}  // namespace felicia