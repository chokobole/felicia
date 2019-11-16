// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_JS_MASTER_MASTER_CLIENT_H_
#define FELICIA_JS_MASTER_MASTER_CLIENT_H_

#if defined(FEL_WIN_NO_GRPC)

#include "napi.h"

#include "third_party/chromium/base/macros.h"

#include "felicia/core/master/master_client_interface.h"

namespace felicia {

class MasterClient : public MasterClientInterface {
 public:
  MasterClient();
  ~MasterClient();

  Status Start() override;
  Status Stop() override;

#define MASTER_METHOD(Method, method, cancelable)                         \
  void Method##Async(const Method##Request* request,                      \
                     Method##Response* response, StatusOnceCallback done) \
      override;
#include "felicia/core/master/rpc/master_method_list.h"
#undef MASTER_METHOD

 private:
  Napi::ObjectReference master_client_;

  DISALLOW_COPY_AND_ASSIGN(MasterClient);
};

}  // namespace felicia

#endif  // defined(FEL_WIN_NO_GRPC)

#endif  // FELICIA_JS_MASTER_MASTER_CLIENT_H_