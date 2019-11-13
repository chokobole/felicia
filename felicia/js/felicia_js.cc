// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "napi.h"

#include "felicia/js/master_proxy_js.h"
#include "felicia/js/settings_js.h"
#include "felicia/js/status_js.h"

namespace felicia {

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  JsStatus::Init(env, exports);
  communication::JsSettings::Init(env, exports);
  JsMasterProxy::Init(env, exports);

  return exports;
}

NODE_API_MODULE(felicia_js, Init)

}  // namespace felicia