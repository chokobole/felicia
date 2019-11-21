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

#ifndef FELIICA_CORE_RPC_GRPC_CLIENT_IMPL_H_
#define FELIICA_CORE_RPC_GRPC_CLIENT_IMPL_H_

#include <memory>

#include "grpcpp/grpcpp.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/base/threading/thread.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/protobuf/channel.pb.h"
#include "felicia/core/rpc/client_interface.h"
#include "felicia/core/rpc/grpc_async_client_call.h"
#include "felicia/core/rpc/grpc_client_cq_tag.h"
#include "felicia/core/rpc/grpc_util.h"

namespace felicia {
namespace rpc {

#define FEL_GRPC_CLIENT Client<T, std::enable_if_t<IsGrpcService<T>::value>>

template <typename T>
class FEL_GRPC_CLIENT : public ClientInterface {
 public:
  typedef T GrpcService;

  static std::string service_name() { return GrpcService::service_full_name(); }

  Client() = default;
  explicit Client(std::shared_ptr<::grpc::Channel> channel);
  ~Client() override = default;

  void Connect(const IPEndPoint& ip_endpoint,
               StatusOnceCallback callback) override;

  // Non-blocking
  Status Run() override {
    RunRpcsLoops(1);
    return Status::OK();
  }

  Status Shutdown() override {
    ShutdownClient();
    return Status::OK();
  }

  void WaitUntilShutdown() { threads_.clear(); }

  void HandleRpcsLoop();

 protected:
  typedef typename GrpcService::Stub Stub;

  virtual std::shared_ptr<::grpc::Channel> ConnectToGrpcServer(
      const std::string& ip, uint16_t port);

  void RunRpcsLoops(int num_threads);
  void ShutdownClient();

  std::unique_ptr<Stub> stub_;
  std::unique_ptr<::grpc::CompletionQueue> cq_;
  std::vector<std::unique_ptr<base::Thread>> threads_;

  DISALLOW_COPY_AND_ASSIGN(Client);
};

template <typename T>
FEL_GRPC_CLIENT::Client(std::shared_ptr<::grpc::Channel> channel)
    : stub_(GrpcService::NewStub(channel)),
      cq_(std::make_unique<::grpc::CompletionQueue>()) {}

template <typename T>
void FEL_GRPC_CLIENT::Connect(const IPEndPoint& ip_endpoint,
                              StatusOnceCallback callback) {
  auto channel = ConnectToGrpcServer(ip_endpoint.ip(), ip_endpoint.port());
  stub_ = GrpcService::NewStub(channel);
  cq_ = std::make_unique<::grpc::CompletionQueue>();
  std::move(callback).Run(Status::OK());
}

template <typename T>
void FEL_GRPC_CLIENT::HandleRpcsLoop() {
  void* tag;
  bool ok;
  while (cq_->Next(&tag, &ok)) {
    GrpcClientCQTag* callback_tag = static_cast<GrpcClientCQTag*>(tag);
    callback_tag->OnCompleted(ok);
  }
}

template <typename T>
std::shared_ptr<::grpc::Channel> FEL_GRPC_CLIENT::ConnectToGrpcServer(
    const std::string& ip, uint16_t port) {
  auto channel =
      ::grpc::CreateChannel(base::StringPrintf("%s:%d", ip.c_str(), port),
                            ::grpc::InsecureChannelCredentials());
  if (!channel->WaitForConnected(
          gpr_time_add(gpr_now(GPR_CLOCK_REALTIME),
                       gpr_time_from_seconds(10, GPR_TIMESPAN)))) {
    LOG(ERROR) << "Failed to connect to grpc server on port " << port;
    return nullptr;
  }

  return channel;
}

template <typename T>
void FEL_GRPC_CLIENT::RunRpcsLoops(int num_threads) {
  for (int i = 0; i < num_threads; ++i) {
    threads_.push_back(std::make_unique<base::Thread>(
        base::StringPrintf("%s RPC Loop%d", service_name().c_str(), (i + 1))));
  }

  std::for_each(threads_.begin(), threads_.end(),
                [this](const std::unique_ptr<base::Thread>& thread) {
                  thread->Start();
                  thread->task_runner()->PostTask(
                      FROM_HERE, base::BindOnce(&Client::HandleRpcsLoop,
                                                base::Unretained(this)));
                });
}

template <typename T>
void FEL_GRPC_CLIENT::ShutdownClient() {
  cq_->Shutdown();
  threads_.clear();
}

#define FEL_GRPC_CLIENT_METHOD_DECLARE(method)       \
  void method##Async(const method##Request* request, \
                     method##Response* response, StatusOnceCallback done)

#define FEL_GRPC_CLIENT_METHOD_DEFINE(clazz, method)                  \
  void clazz::method##Async(const method##Request* request,           \
                            method##Response* response,               \
                            StatusOnceCallback done) {                \
    new GrpcAsyncClientCall<Stub, method##Request, method##Response>( \
        stub_.get(), request, response, &Stub::PrepareAsync##method,  \
        cq_.get(), std::move(done));                                  \
  }

}  // namespace rpc
}  // namespace felicia

#endif  // FELIICA_CORE_RPC_GRPC_CLIENT_IMPL_H_