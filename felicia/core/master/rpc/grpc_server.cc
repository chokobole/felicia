#include "felicia/core/master/rpc/grpc_server.h"

#include "grpcpp/grpcpp.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/base/threading/thread.h"

#include "felicia/core/master/rpc/grpc_util.h"

namespace felicia {

Status GrpcServer::Init() {
  std::string ip = ResolveGRPCServiceIp().ToString();
  uint16_t port = ResolveGRPCServicePort();
  std::cout << "Running on " << ip << ":" << port << "..." << std::endl;
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

  std::vector<std::unique_ptr<::base::Thread>> threads;
  threads.push_back(std::make_unique<::base::Thread>("RPC Loop1"));
  threads.push_back(std::make_unique<::base::Thread>("RPC Loop2"));

  std::for_each(
      threads.begin(), threads.end(),
      [this](const std::unique_ptr<::base::Thread>& thread) {
        thread->Start();
        thread->task_runner()->PostTask(
            FROM_HERE,
            ::base::BindOnce(&GrpcMasterService::HandleRpcsLoop,
                             ::base::Unretained(master_service_.get())));
      });

  return Status::OK();
}

}  // namespace felicia