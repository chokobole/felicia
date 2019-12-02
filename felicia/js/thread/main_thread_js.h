// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_JS_THREAD_MAIN_THREAD_JS_H_
#define FELICIA_JS_THREAD_MAIN_THREAD_JS_H_

#if defined(FEL_NODE_BINDING)

#include "napi.h"

namespace felicia {

class JsMainThread : public Napi::ObjectWrap<JsMainThread> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  JsMainThread(const Napi::CallbackInfo& info);

  void Run(const Napi::CallbackInfo& info);
  void RunBackground(const Napi::CallbackInfo& info);
  void Stop(const Napi::CallbackInfo& info);

 private:
  static Napi::FunctionReference constructor_;
};

}  // namespace felicia

#endif  // defined(FEL_NODE_BINDING)

#endif  // FELICIA_JS_THREAD_MAIN_THREAD_JS_H_