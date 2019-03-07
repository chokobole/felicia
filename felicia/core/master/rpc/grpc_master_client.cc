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
  std::for_each(threads_.begin(), threads_.end(),
                [this](const std::unique_ptr<::base::Thread>& thread) {
                  thread->Stop();
                });
  return Status::OK();
}

Status GrpcMasterClient::Shutdown() {
  cq_.Shutdown();
  return Status::OK();
}

}  // namespace felicia