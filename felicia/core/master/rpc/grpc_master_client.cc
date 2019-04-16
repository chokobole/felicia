#include "felicia/core/master/rpc/grpc_master_client.h"

#include "third_party/chromium/base/bind.h"

namespace felicia {

GrpcMasterClient::GrpcMasterClient(std::shared_ptr<::grpc::Channel> channel)
    : stub_(grpc::MasterService::NewStub(channel)) {}

GrpcMasterClient::~GrpcMasterClient() = default;

Status GrpcMasterClient::Start() {
  threads_.push_back(std::make_unique<::base::Thread>("RPC Loop1"));
  threads_.push_back(std::make_unique<::base::Thread>("RPC Loop2"));

  std::for_each(threads_.begin(), threads_.end(),
                [this](const std::unique_ptr<::base::Thread>& thread) {
                  thread->Start();
                  thread->task_runner()->PostTask(
                      FROM_HERE,
                      ::base::BindOnce(&GrpcMasterClient::HandleRpcsLoop,
                                       ::base::Unretained(this)));
                });

  return Status::OK();
}

Status GrpcMasterClient::Stop() {
  cq_.Shutdown();
  return Status::OK();
}

#define CLIENT_METHOD(method)                                           \
  void GrpcMasterClient::method##Async(const method##Request* request,  \
                                       method##Response* response,      \
                                       StatusOnceCallback done) {       \
    new GrpcAsyncClientCall<grpc::MasterService::Stub, method##Request, \
                            method##Response>(                          \
        stub_.get(), request, response,                                 \
        &grpc::MasterService::Stub::PrepareAsync##method, &cq_,         \
        std::move(done));                                               \
  }

CLIENT_METHOD(RegisterClient)
CLIENT_METHOD(ListClients)
CLIENT_METHOD(RegisterNode)
CLIENT_METHOD(UnregisterNode)
CLIENT_METHOD(ListNodes)
CLIENT_METHOD(PublishTopic)
CLIENT_METHOD(UnpublishTopic)
CLIENT_METHOD(SubscribeTopic)
CLIENT_METHOD(UnsubscribeTopic)
CLIENT_METHOD(ListTopics)

#undef CLIENT_METHOD

void GrpcMasterClient::HandleRpcsLoop() {
  void* tag;
  bool ok;
  while (cq_.Next(&tag, &ok)) {
    GrpcClientCQTag* callback_tag = static_cast<GrpcClientCQTag*>(tag);
    callback_tag->OnCompleted(ok);
  }
}

}  // namespace felicia