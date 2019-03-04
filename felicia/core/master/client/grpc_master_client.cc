#include "felicia/core/master/client/grpc_master_client.h"

namespace felicia {

GrpcMasterClient::GrpcMasterClient(std::shared_ptr<::grpc::Channel> channel)
    : stub_(grpc::MasterService::NewStub(channel)), thread_(&cq_) {}

GrpcMasterClient::~GrpcMasterClient() = default;

Status GrpcMasterClient::Start() {
  ::base::PlatformThread::Create(0, &thread_, &handle_);
  return Status::OK();
}

Status GrpcMasterClient::Join() {
  ::base::PlatformThread::Join(handle_);
  return Status::OK();
}

Status GrpcMasterClient::Shutdown() {
  cq_.Shutdown();
  return Status::OK();
}

}  // namespace felicia