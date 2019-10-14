#ifndef FELIICA_CORE_RPC_CLIENT_H_
#define FELIICA_CORE_RPC_CLIENT_H_

#include <memory>

#include "grpcpp/grpcpp.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/base/threading/thread.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/protobuf/channel.pb.h"
#include "felicia/core/rpc/grpc_async_client_call.h"
#include "felicia/core/rpc/grpc_client_cq_tag.h"

namespace felicia {
namespace rpc {

template <typename GrpcService>
class Client {
 public:
  static std::string service_name() { return GrpcService::service_full_name(); }

  Client() = default;
  explicit Client(std::shared_ptr<::grpc::Channel> channel);
  virtual ~Client() = default;

  virtual Status ConnectAndRun(const IPEndPoint& ip_endpoint);

  // Non-blocking
  virtual Status Run() {
    RunRpcsLoops(1);
    return Status::OK();
  }

  virtual Status Shutdown() {
    ShutdownClient();
    return Status::OK();
  }

  void HandleRpcsLoop();

 protected:
  typedef typename GrpcService::Stub Stub;

  void RunRpcsLoops(int num_threads);
  void ShutdownClient();

  std::unique_ptr<Stub> stub_;
  std::unique_ptr<::grpc::CompletionQueue> cq_;
  std::vector<std::unique_ptr<base::Thread>> threads_;

  DISALLOW_COPY_AND_ASSIGN(Client<GrpcService>);
};

template <typename GrpcService>
Client<GrpcService>::Client(std::shared_ptr<::grpc::Channel> channel)
    : stub_(GrpcService::NewStub(channel)),
      cq_(std::make_unique<::grpc::CompletionQueue>()) {}

template <typename GrpcService>
Status Client<GrpcService>::ConnectAndRun(const IPEndPoint& ip_endpoint) {
  auto channel = ConnectToGrpcServer(ip_endpoint.ip(), ip_endpoint.port());
  stub_ = GrpcService::NewStub(channel);
  cq_ = std::make_unique<::grpc::CompletionQueue>();
  return Run();
}

template <typename GrpcService>
void Client<GrpcService>::HandleRpcsLoop() {
  void* tag;
  bool ok;
  while (cq_->Next(&tag, &ok)) {
    GrpcClientCQTag* callback_tag = static_cast<GrpcClientCQTag*>(tag);
    callback_tag->OnCompleted(ok);
  }
}

template <typename GrpcService>
void Client<GrpcService>::RunRpcsLoops(int num_threads) {
  for (int i = 0; i < num_threads; ++i) {
    threads_.push_back(std::make_unique<base::Thread>(
        base::StringPrintf("%s RPC Loop%d", service_name().c_str(), (i + 1))));
  }

  std::for_each(threads_.begin(), threads_.end(),
                [this](const std::unique_ptr<base::Thread>& thread) {
                  thread->Start();
                  thread->task_runner()->PostTask(
                      FROM_HERE,
                      base::BindOnce(&Client<GrpcService>::HandleRpcsLoop,
                                     base::Unretained(this)));
                });
}

template <typename GrpcService>
void Client<GrpcService>::ShutdownClient() {
  cq_->Shutdown();
  threads_.clear();
}

#define FEL_CLIENT_METHOD_DECLARE(method)            \
  void method##Async(const method##Request* request, \
                     method##Response* response, StatusOnceCallback done)

#define FEL_CLIENT_METHOD_DEFINE(clazz, method)                       \
  void clazz::method##Async(const method##Request* request,           \
                            method##Response* response,               \
                            StatusOnceCallback done) {                \
    new GrpcAsyncClientCall<Stub, method##Request, method##Response>( \
        stub_.get(), request, response, &Stub::PrepareAsync##method,  \
        cq_.get(), std::move(done));                                  \
  }

}  // namespace rpc
}  // namespace felicia

#endif  // FELIICA_CORE_RPC_CLIENT_H_