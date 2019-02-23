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

// void GrpcMasterClient::RegisterNodeAsync(const RegisterNodeRequest* request,
//                                          RegisterNodeResponse* response,
//                                          StatusCallback done) {
//   new GrpcAsyncClientCall<grpc::MasterService::Stub, RegisterNodeRequest,
//                           RegisterNodeResponse>(
//       stub_.get(), request, response,
//       &grpc::MasterService::Stub::PrepareAsyncRegisterNode, &cq_, done);
// }

// void GrpcMasterClient::GetNodesAsync(const GetNodesRequest* request,
//                                     GetNodesResponse* response,
//                                     StatusCallback done) {
//   new GrpcAsyncClientCall<grpc::MasterService::Stub, GetNodesRequest,
//                     GetNodesResponse>(
//       stub_.get(), request, response,
//       &grpc::MasterService::Stub::PrepareAsyncGetNodes, &cq_, done);
// }

// void GrpcMasterClient::PublishTopicAsync(const PublishTopicRequest* request,
//                                          PublishTopicResponse* response,
//                                          StatusCallback done) {
//   new GrpcAsyncClientCall<grpc::MasterService::Stub, PublishTopicRequest,
//                     PublishTopicResponse>(
//       stub_.get(), request, response,
//       &grpc::MasterService::Stub::PrepareAsyncPublishTopic, &cq_, done);
// }

// void GrpcMasterClient::SubscribeTopicAsync(const SubscribeTopicRequest*
// request,
//                                            SubscribeTopicResponse* response,
//                                            StatusCallback done) {
//   new GrpcAsyncClientCall<grpc::MasterService::Stub, SubscribeTopicRequest,
//                     SubscribeTopicResponse>(
//       stub_.get(), request, response,
//       &grpc::MasterService::Stub::PrepareAsyncSubscribeTopic, &cq_, done);
// }

}  // namespace felicia