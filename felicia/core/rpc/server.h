#ifndef FELIICA_CORE_RPC_SERVER_H_
#define FELIICA_CORE_RPC_SERVER_H_

#include <memory>

#include "grpcpp/grpcpp.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/threading/thread.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/lib/net/net_util.h"
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {
namespace rpc {

template <typename Service>
class Server {
 public:
  Server() = default;
  ~Server() = default;

  ChannelDef channel_def() const {
    ChannelDef channel_def;
    channel_def.set_type(ChannelDef::CHANNEL_TYPE_TCP);
    IPEndPoint* ip_endpoint = channel_def.mutable_ip_endpoint();
    ip_endpoint->set_ip(HostIPAddress(HOST_IP_ONLY_ALLOW_IPV4).ToString());
    ip_endpoint->set_port(port_);
    return channel_def;
  }

  virtual Status Start();

  // Blocking
  virtual Status Run() {
    RunRpcsLoops(1);
    return Status::OK();
  }

  virtual Status Shutdown() {
    ShutdownServer();
    return Status::OK();
  }

 protected:
  // You need to set |port_| to server port here.
  virtual std::string ConfigureServerAddress() {
    port_ = PickRandomPort(true);
    return base::StringPrintf("[::]:%d", static_cast<int>(port_));
  }

  virtual Status RegisterService(::grpc::ServerBuilder* builder) = 0;

  void RunRpcsLoops(int num_threads);
  void ShutdownServer();

  std::unique_ptr<Service> service_;
  std::unique_ptr<::grpc::Server> server_;
  uint16_t port_;
};

template <typename Service>
Status Server<Service>::Start() {
  ::grpc::ServerBuilder builder;
  builder.AddListeningPort(ConfigureServerAddress(),
                           ::grpc::InsecureServerCredentials());
  builder.SetMaxMessageSize(std::numeric_limits<int32_t>::max());
  Status s = RegisterService(&builder);
  if (!s.ok()) return s;
  server_ = builder.BuildAndStart();

  if (!server_) {
    return errors::Unknown("Could not start gRPC server");
  }

  return Status::OK();
}

template <typename Service>
void Server<Service>::RunRpcsLoops(int num_threads) {
  std::vector<std::unique_ptr<base::Thread>> threads;
  for (int i = 0; i < num_threads; ++i) {
    threads.push_back(std::make_unique<base::Thread>(base::StringPrintf(
        "%s RPC Loop%d", Service::service_name().c_str(), (i + 1))));
  }

  std::for_each(threads.begin(), threads.end(),
                [this](const std::unique_ptr<base::Thread>& thread) {
                  thread->Start();
                  thread->task_runner()->PostTask(
                      FROM_HERE,
                      base::BindOnce(&MasterService::HandleRpcsLoop,
                                     base::Unretained(service_.get())));
                });
}

template <typename Service>
void Server<Service>::ShutdownServer() {
  server_->Shutdown();
  service_->Shutdown();
}

}  // namespace rpc
}  // namespace felicia

#endif  // FELIICA_CORE_RPC_SERVER_H_