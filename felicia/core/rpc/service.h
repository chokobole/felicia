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

#ifndef FELICIA_CORE_RPC_SERVICE_H_
#define FELICIA_CORE_RPC_SERVICE_H_

#include "grpcpp/alarm.h"
#include "grpcpp/grpcpp.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/synchronization/lock.h"
#include "third_party/chromium/base/thread_annotations.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/rpc/grpc_call.h"
#include "felicia/core/rpc/grpc_util.h"

namespace felicia {
namespace rpc {

template <typename GrpcService>
class Service {
 public:
  static std::string service_name() { return GrpcService::service_full_name(); }

  explicit Service(::grpc::ServerBuilder* builder);
  virtual ~Service() = default;

  void Shutdown();
  void HandleRpcsLoop();

 protected:
  typedef typename GrpcService::AsyncService GrpcAsyncService;

  template <typename ServiceImpl, typename RequestMessage,
            typename ResponseMessage>
  using GrpcCall =
      Call<ServiceImpl, GrpcAsyncService, RequestMessage, ResponseMessage>;

  virtual void EnqueueRequests() = 0;

  template <typename GrpcCall>
  static void OnHandleRequest(GrpcCall* call, const Status& status) {
    call->SendResponse(ToGrpcStatus(status));
  }

  std::unique_ptr<::grpc::ServerCompletionQueue> cq_;
  GrpcAsyncService async_service_;

  base::Lock lock_;
  bool is_shutdown_ GUARDED_BY(lock_);
  ::grpc::Alarm* shutdown_alarm_ = nullptr;
};

template <typename GrpcService>
Service<GrpcService>::Service(::grpc::ServerBuilder* builder)
    : is_shutdown_(false) {
  builder->RegisterService(&async_service_);
  cq_ = builder->AddCompletionQueue();
}

template <typename GrpcService>
void Service<GrpcService>::Shutdown() {
  bool did_shutdown = false;
  {
    base::AutoLock l(lock_);
    if (!is_shutdown_) {
      LOG(INFO) << "Shutting down " << service_name() << "...";
      is_shutdown_ = true;
      did_shutdown = true;
    }
  }
  if (did_shutdown) {
    // NOTE(mrry): This enqueues a special event (with a null tag)
    // that causes the completion queue to be shut down on the
    // polling thread.
    shutdown_alarm_ =
        new ::grpc::Alarm(cq_.get(), gpr_now(GPR_CLOCK_MONOTONIC), nullptr);
  }
}

template <typename GrpcService>
void Service<GrpcService>::HandleRpcsLoop() {
  typedef typename UntypedCall<Service<GrpcService>>::Tag Tag;
  void* tag;
  bool ok;
  EnqueueRequests();
  while (cq_->Next(&tag, &ok)) {
    Tag* callback_tag = static_cast<Tag*>(tag);
    if (callback_tag) {
      callback_tag->OnCompleted(this, ok);
    } else {
      // NOTE(mrry): A null `callback_tag` indicates that this is
      // the shutdown alarm.
      cq_->Shutdown();
    }
  }
}

#define FEL_ENQUEUE_REQUEST(clazz, method, supports_cancel)               \
  do {                                                                    \
    base::AutoLock l(lock_);                                              \
    if (!is_shutdown_) {                                                  \
      GrpcCall<clazz, method##Request, method##Response>::EnqueueRequest( \
          &async_service_, cq_.get(), &GrpcAsyncService::Request##method, \
          &clazz::Handle##method, supports_cancel);                       \
    }                                                                     \
  } while (0)

#define FEL_GRPC_SERVICE_METHOD_DECLARE(clazz, method) \
  void Handle##method(GrpcCall<clazz, method##Request, method##Response>* call)

#define FEL_GRPC_SERVICE_METHOD_DEFINE(clazz, instance, method,      \
                                       supports_cancel)              \
  void clazz::Handle##method(                                        \
      GrpcCall<clazz, method##Request, method##Response>* call) {    \
    instance->method(                                                \
        &call->request_, &call->response_,                           \
        base::BindOnce(                                              \
            &clazz::OnHandleRequest<                                 \
                GrpcCall<clazz, method##Request, method##Response>>, \
            call));                                                  \
    FEL_ENQUEUE_REQUEST(clazz, method, supports_cancel);             \
  }

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_CORE_RPC_SERVICE_H_