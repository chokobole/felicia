#include "felicia/cc/master_proxy.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/master/rpc/grpc_util.h"
#include "felicia/core/platform/net_util.h"

namespace felicia {

MasterProxy::MasterProxy()
    : message_loop_(
          std::make_unique<::base::MessageLoop>(::base::MessageLoop::TYPE_IO)),
      run_loop_(std::make_unique<::base::RunLoop>()),
      topic_source_watcher_(this) {
  auto channel = ConnectGRPCService();
  master_client_ = std::make_unique<GrpcMasterClient>(channel);
  master_client_->Start();

  IPEndPoint* ip_endpoint = node_info_.mutable_ip_endpoint();
  ip_endpoint->set_ip(net::HostIPAddress(net::HOST_IP_ONLY_ALLOW_IPV4));
  ip_endpoint->set_port(net::PickRandomPort(true));

  LOG(INFO) << "MasterProxy Listening on: " << ip_endpoint->DebugString();

  topic_source_watcher_.set_node_info(node_info_);
}

void MasterProxy::Run() {
  run_loop_->Run();
  master_client_->Join();
}

MasterProxy& MasterProxy::GetInstance() {
  static ::base::NoDestructor<MasterProxy> master_proxy;
  return *master_proxy;
}

void MasterProxy::PublishTopicAsync(PublishTopicRequest* request,
                                    PublishTopicResponse* response,
                                    StatusCallback callback) {
  master_client_->PublishTopicAsync(request, response, callback);
}

void MasterProxy::SubscribeTopicAsync(
    SubscribeTopicRequest* request, SubscribeTopicResponse* response,
    StatusCallback callback,
    TopicSourceWatcher::OnNewTopicSourceCallback callback2) {
  topic_source_watcher_.Listen(request->topic(), callback2);
  master_client_->SubscribeTopicAsync(request, response, callback);
}

}  // namespace felicia