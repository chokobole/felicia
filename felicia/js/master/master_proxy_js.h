// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_JS_MASTER_MASTER_PROXY_JS_H_
#define FELICIA_JS_MASTER_MASTER_PROXY_JS_H_

#if defined(FEL_NODE_BINDING)

#include "napi.h"

namespace felicia {

class JsMasterProxy : public Napi::ObjectWrap<JsMasterProxy> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  JsMasterProxy(const Napi::CallbackInfo& info);

#if defined(FEL_WIN_NODE_BINDING)
  Napi::Value StartMasterClient(const Napi::CallbackInfo& info);
  Napi::Value is_client_info_set(const Napi::CallbackInfo& info);
#endif  // defined(FEL_WIN_NODE_BINDING)

  Napi::Value Start(const Napi::CallbackInfo& info);
  Napi::Value Stop(const Napi::CallbackInfo& info);

  void RequestRegisterTopicInfoWatcherNode(const Napi::CallbackInfo& info);

 private:
  static Napi::FunctionReference constructor_;
};

}  // namespace felicia

#endif  // defined(FEL_NODE_BINDING)

#endif  // FELICIA_JS_MASTER_MASTER_PROXY_JS_H_
