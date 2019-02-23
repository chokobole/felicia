/* Copyright 2016 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
// Modifications copyright (C) 2019 felicia

#ifndef FELICIA_CORE_MASTER_GRPC_CLEINT_CQ_TAG_
#define FELICIA_CORE_MASTER_GRPC_CLEINT_CQ_TAG_

#include "third_party/chromium/base/macros.h"

namespace felicia {

// Represents a pending asynchronous client call as a tag that can be
// stored in a `grpc::CompletionQueue`.
class GrpcClientCQTag {
 public:
  GrpcClientCQTag() {}
  virtual ~GrpcClientCQTag() {}

  // OnCompleted is invoked when the RPC has finished.
  // Implementations of OnCompleted must delete *this.
  virtual void OnCompleted(bool ok) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(GrpcClientCQTag);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_GRPC_CLEINT_CQ_TAG_