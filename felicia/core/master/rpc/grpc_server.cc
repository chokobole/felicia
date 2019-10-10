#include "felicia/core/master/rpc/grpc_server.h"

#include <csignal>

#include "grpcpp/grpcpp.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/base/threading/thread.h"

#include "felicia/core/master/rpc/grpc_info.h"
#include "felicia/core/master/rpc/grpc_util.h"

namespace felicia {

namespace {

GrpcServer* g_grpc_server = nullptr;

void ShutdownGrpcServer(int signal) {
  if (g_grpc_server) {
    g_grpc_server->Shutdown();
    g_grpc_server = nullptr;
  }
}

}  // namespace

Status GrpcServer::Init() {
  std::string ip = ResolveGRPCServiceIp().ToString();
  uint16_t port = ResolveGRPCServicePort();
  std::cout << "Running on " << ip << ":" << port << "..." << std::endl;
  std::string server_address(base::StringPrintf("[::]:%d", port));

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
  RegisterSignals();
  master_->Run();

  std::vector<std::unique_ptr<base::Thread>> threads;
  threads.push_back(std::make_unique<base::Thread>("RPC Loop1"));
  threads.push_back(std::make_unique<base::Thread>("RPC Loop2"));

  std::for_each(threads.begin(), threads.end(),
                [this](const std::unique_ptr<base::Thread>& thread) {
                  thread->Start();
                  thread->task_runner()->PostTask(
                      FROM_HERE,
                      base::BindOnce(&GrpcMasterService::HandleRpcsLoop,
                                     base::Unretained(master_service_.get())));
                });

  return Status::OK();
}

Status GrpcServer::Shutdown() {
  server_->Shutdown();
  master_service_->Shutdown();
  master_->Stop();

  if (!on_shutdown_callback_.is_null()) std::move(on_shutdown_callback_).Run();

  return Status::OK();
}

void GrpcServer::RegisterSignals() {
  g_grpc_server = this;
  // To handle general case when POSIX ask the process to quit.
  std::signal(SIGTERM, &felicia::ShutdownGrpcServer);
  // To handle Ctrl + C.
  std::signal(SIGINT, &felicia::ShutdownGrpcServer);
#if defined(OS_POSIX)
  // To handle when the terminal is closed.
  std::signal(SIGHUP, &felicia::ShutdownGrpcServer);
#endif
}

}  // namespace felicia