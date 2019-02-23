#include "felicia/core/master/rpc/grpc_server.h"

#include "grpcpp/grpcpp.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/master/rpc/grpc_util.h"

namespace felicia {

Status GrpcServer::Init() {
  uint16_t port = ResolveGRPCServicePort();
  std::string server_address(::base::StringPrintf("[::]:%d", port));

  ::grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
  builder.SetMaxMessageSize(std::numeric_limits<int32_t>::max());
  master_ = std::unique_ptr<Master>(new Master());
  master_service_ = std::unique_ptr<GrpcMasterService>(
      new GrpcMasterService(master_.get(), &builder));
  server_ = builder.BuildAndStart();

  if (!server_) {
    return errors::Unknown("Could not start gRPC server");
  }

  return Status::OK();
}

Status GrpcServer::Start() {
  master_->Run();
  master_service_->HandleRpcsLoop();
  return Status::OK();
}

}  // namespace felicia