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

void GrpcMasterService::HandleRegisterClient(
    MasterCall<RegisterClientRequest, RegisterClientResponse>* call) {
  master_->RegisterClient(
      &call->request_, &call->response_,
      ::base::BindOnce(
          &OnHandleRequest<
              MasterCall<RegisterClientRequest, RegisterClientResponse>>,
          call));
  ENQUEUE_REQUEST(RegisterClient, true);
}

void GrpcMasterService::HandleRegisterNode(
    MasterCall<RegisterNodeRequest, RegisterNodeResponse>* call) {
  master_->RegisterNode(
      &call->request_, &call->response_,
      ::base::BindOnce(
          &OnHandleRequest<
              MasterCall<RegisterNodeRequest, RegisterNodeResponse>>,
          call));
  ENQUEUE_REQUEST(RegisterNode, true);
}

void GrpcMasterService::HandleListNodes(
    MasterCall<ListNodesRequest, ListNodesResponse>* call) {
  master_->ListNodes(
      &call->request_, &call->response_,
      ::base::BindOnce(
          &OnHandleRequest<MasterCall<ListNodesRequest, ListNodesResponse>>,
          call));
  ENQUEUE_REQUEST(ListNodes, false);
}

void GrpcMasterService::HandlePublishTopic(
    MasterCall<PublishTopicRequest, PublishTopicResponse>* call) {
  master_->PublishTopic(
      &call->request_, &call->response_,
      ::base::BindOnce(
          &OnHandleRequest<
              MasterCall<PublishTopicRequest, PublishTopicResponse>>,
          call));
  ENQUEUE_REQUEST(PublishTopic, true);
}

void GrpcMasterService::HandleSubscribeTopic(
    MasterCall<SubscribeTopicRequest, SubscribeTopicResponse>* call) {
  master_->SubscribeTopic(
      &call->request_, &call->response_,
      ::base::BindOnce(
          &OnHandleRequest<
              MasterCall<SubscribeTopicRequest, SubscribeTopicResponse>>,
          call));
  ENQUEUE_REQUEST(SubscribeTopic, true);
}

void GrpcMasterService::HandleListTopics(
    MasterCall<ListTopicsRequest, ListTopicsResponse>* call) {
  master_->ListTopics(
      &call->request_, &call->response_,
      ::base::BindOnce(
          &OnHandleRequest<MasterCall<ListTopicsRequest, ListTopicsResponse>>,
          call));
  ENQUEUE_REQUEST(ListTopics, false);
}

#undef ENQUEUE_REQUEST

}  // namespace felicia