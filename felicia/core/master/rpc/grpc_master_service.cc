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

#include "felicia/core/master/rpc/grpc_master_service.h"

#include "third_party/chromium/base/bind.h"

#include "felicia/core/master/rpc/grpc_call.h"
#include "felicia/core/master/rpc/grpc_util.h"

namespace felicia {

GrpcMasterService::GrpcMasterService(Master* master,
                                     ::grpc::ServerBuilder* builder)
    : master_(master), is_shutdown_(false) {
  builder->RegisterService(&master_service_);
  cq_ = builder->AddCompletionQueue();
}

void GrpcMasterService::Shutdown() {
  bool did_shutdown = false;
  {
    ::base::AutoLock l(lock_);
    if (!is_shutdown_) {
      LOG(INFO) << "Shutting down GrpcMasterService.";
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

#define ENQUEUE_REQUEST(method, supports_cancel)                              \
  do {                                                                        \
    ::base::AutoLock l(lock_);                                                \
    if (!is_shutdown_) {                                                      \
      Call<GrpcMasterService, grpc::MasterService::AsyncService,              \
           method##Request, method##Response>::                               \
          EnqueueRequest(&master_service_, cq_.get(),                         \
                         &grpc::MasterService::AsyncService::Request##method, \
                         &GrpcMasterService::Handle##method,                  \
                         (supports_cancel));                                  \
    }                                                                         \
  } while (0)

void GrpcMasterService::HandleRpcsLoop() {
  void* tag;
  bool ok;
  ENQUEUE_REQUEST(RegisterClient, true);
  ENQUEUE_REQUEST(ListClients, false);
  ENQUEUE_REQUEST(RegisterNode, true);
  ENQUEUE_REQUEST(ListNodes, false);
  ENQUEUE_REQUEST(PublishTopic, true);
  ENQUEUE_REQUEST(SubscribeTopic, true);
  ENQUEUE_REQUEST(ListTopics, false);
  while (cq_->Next(&tag, &ok)) {
    UntypedCall<GrpcMasterService>::Tag* callback_tag =
        static_cast<UntypedCall<GrpcMasterService>::Tag*>(tag);
    if (callback_tag) {
      callback_tag->OnCompleted(this, ok);
    } else {
      // NOTE(mrry): A null `callback_tag` indicates that this is
      // the shutdown alarm.
      cq_->Shutdown();
    }
  }
}

namespace {

template <typename CallTy>
void OnHandleRequest(CallTy* call, const Status& status) {
  call->SendResponse(ToGrpcStatus(status));
}

}  // namespace

#define SERVICE_METHOD(method, supports_cancel)                              \
  void GrpcMasterService::Handle##method(                                    \
      MasterCall<method##Request, method##Response>* call) {                 \
    master_->method(                                                         \
        &call->request_, &call->response_,                                   \
        ::base::BindOnce(                                                    \
            &OnHandleRequest<MasterCall<method##Request, method##Response>>, \
            call));                                                          \
    ENQUEUE_REQUEST(method, supports_cancel);                                \
  }

SERVICE_METHOD(RegisterClient, true)
SERVICE_METHOD(ListClients, false)
SERVICE_METHOD(RegisterNode, true)
SERVICE_METHOD(ListNodes, false)
SERVICE_METHOD(PublishTopic, true)
SERVICE_METHOD(SubscribeTopic, true)
SERVICE_METHOD(ListTopics, false)

#undef SERVICE_METHOD
#undef ENQUEUE_REQUEST

}  // namespace felicia