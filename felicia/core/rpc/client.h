#ifndef FELIICA_CORE_RPC_CLIENT_H_
#define FELIICA_CORE_RPC_CLIENT_H_

#include <memory>

#include "grpcpp/grpcpp.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/base/threading/thread.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/rpc/grpc_async_client_call.h"
#include "felicia/core/rpc/grpc_client_cq_tag.h"

namespace felicia {
namespace rpc {

template <typename GrpcService>
class Client {
 public:
  static std::string service_name() { return GrpcService::service_full_name(); }

  explicit Client(std::shared_ptr<::grpc::Channel> channel);
  ~Client() = default;

  // Non-blocking
  Status Run();

  Status Shutdown();

  void HandleRpcsLoop();

 protected:
  typedef typename GrpcService::Stub Stub;

  std::unique_ptr<Stub> stub_;
  ::grpc::CompletionQueue cq_;
  std::vector<std::unique_ptr<base::Thread>> threads_;

  DISALLOW_COPY_AND_ASSIGN(Client<GrpcService>);
};

template <typename GrpcService>
Client<GrpcService>::Client(std::shared_ptr<::grpc::Channel> channel)
    : stub_(GrpcService::NewStub(channel)) {}

template <typename GrpcService>
Status Client<GrpcService>::Run() {
  threads_.push_back(std::make_unique<base::Thread>(
      base::StringPrintf("%s RPC Loop", service_name().c_str())));

  std::for_each(threads_.begin(), threads_.end(),
                [this](const std::unique_ptr<base::Thread>& thread) {
                  thread->Start();
                  thread->task_runner()->PostTask(
                      FROM_HERE,
                      base::BindOnce(&Client<GrpcService>::HandleRpcsLoop,
                                     base::Unretained(this)));
                });

  return Status::OK();
}

template <typename GrpcService>
Status Client<GrpcService>::Shutdown() {
  cq_.Shutdown();
  return Status::OK();
}

template <typename GrpcService>
void Client<GrpcService>::HandleRpcsLoop() {
  void* tag;
  bool ok;
  while (cq_.Next(&tag, &ok)) {
    GrpcClientCQTag* callback_tag = static_cast<GrpcClientCQTag*>(tag);
    callback_tag->OnCompleted(ok);
  }
}

#define FEL_CLIENT_METHOD_DECLARE(method)            \
  void method##Async(const method##Request* request, \
                     method##Response* response, StatusOnceCallback done)

#define FEL_CLIENT_METHOD_DEFINE(clazz, method)                            \
  void clazz::method##Async(const method##Request* request,                \
                            method##Response* response,                    \
                            StatusOnceCallback done) {                     \
    new GrpcAsyncClientCall<Stub, method##Request, method##Response>(      \
        stub_.get(), request, response, &Stub::PrepareAsync##method, &cq_, \
        std::move(done));                                                  \
  }

}  // namespace rpc
}  // namespace felicia

#endif  // FELIICA_CORE_RPC_CLIENT_H_