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

#ifndef FELICIA_CORE_MASTER_RPC_GRPC_MASTER_SERVICE_H_
#define FELICIA_CORE_MASTER_RPC_GRPC_MASTER_SERVICE_H_

#include "grpcpp/alarm.h"
#include "grpcpp/grpcpp.h"
#include "third_party/chromium/base/synchronization/lock.h"
#include "third_party/chromium/base/thread_annotations.h"

#include "felicia/core/master/master.h"
#include "felicia/core/master/rpc/grpc_call.h"
#include "felicia/core/master/rpc/master_service.grpc.pb.h"

namespace felicia {

class GrpcMasterService {
 public:
  GrpcMasterService(Master* master, ::grpc::ServerBuilder* builder);

  void Shutdown();
  void HandleRpcsLoop();

 private:
  template <class RequestMessage, class ResponseMessage>
  using MasterCall = Call<GrpcMasterService, grpc::MasterService::AsyncService,
                          RequestMessage, ResponseMessage>;

  void HandleRegisterNode(
      MasterCall<RegisterNodeRequest, RegisterNodeResponse>* call);
  void HandleGetNodes(MasterCall<GetNodesRequest, GetNodesResponse>* call);
  void HandlePublishTopic(
      MasterCall<PublishTopicRequest, PublishTopicResponse>* call);
  void HandleSubscribeTopic(
      MasterCall<SubscribeTopicRequest, SubscribeTopicResponse>* call);

  Master* master_;
  std::unique_ptr<::grpc::ServerCompletionQueue> cq_;
  grpc::MasterService::AsyncService master_service_;

  ::base::Lock lock_;
  bool is_shutdown_ GUARDED_BY(lock_);
  ::grpc::Alarm* shutdown_alarm_ = nullptr;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_RPC_GRPC_MASTER_SERVICE_H_