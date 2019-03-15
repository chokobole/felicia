#include "felicia/core/master/master_proxy.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/lib/net/net_util.h"
#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/master/rpc/grpc_master_client.h"
#include "felicia/core/master/rpc/grpc_util.h"

namespace felicia {

MasterProxy::MasterProxy()
    : message_loop_(
          std::make_unique<::base::MessageLoop>(::base::MessageLoop::TYPE_IO)),
      run_loop_(std::make_unique<::base::RunLoop>()),
      topic_info_watcher_(this),
      heart_beat_signaller_(this) {}

Status MasterProxy::Start() {
  auto channel = ConnectGRPCService();
  master_client_interface_ = std::make_unique<GrpcMasterClient>(channel);
  Status s = master_client_interface_->Start();
  if (!s.ok()) return s;

  heart_beat_signaller_.Start();
  topic_info_watcher_.Start();

  *client_info_.mutable_heart_beat_signaller_source() =
      heart_beat_signaller_.channel_source();
  *client_info_.mutable_topic_info_watcher_source() =
      topic_info_watcher_.channel_source();

  RegisterClient();

  return Status::OK();
}

void MasterProxy::Run() { run_loop_->Run(); }

Status MasterProxy::Stop() {
  Status s = master_client_interface_->Stop();
  run_loop_->Quit();
  return s;
}

MasterProxy& MasterProxy::GetInstance() {
  static ::base::NoDestructor<MasterProxy> master_proxy;
  return *master_proxy;
}

void MasterProxy::RegisterClient() {
  RegisterClientRequest* request = new RegisterClientRequest();
  *request->mutable_client_info() = client_info_;
  RegisterClientResponse* response = new RegisterClientResponse();

  ::base::WaitableEvent* event = new ::base::WaitableEvent;
  master_client_interface_->RegisterClientAsync(
      request, response,
      ::base::BindOnce(&MasterProxy::OnRegisterClient, ::base::Unretained(this),
                       event, ::base::Owned(request), ::base::Owned(response)));
  event->Wait();
  delete event;
}

void MasterProxy::OnRegisterClient(::base::WaitableEvent* event,
                                   RegisterClientRequest* request,
                                   RegisterClientResponse* response,
                                   const Status& s) {
  client_info_.set_id(response->id());
  event->Signal();
}

void MasterProxy::OnRegisterNodeAsync(std::unique_ptr<NodeLifecycle> node,
                                      RegisterNodeRequest* request,
                                      RegisterNodeResponse* response,
                                      const Status& s) {
  if (!s.ok()) {
    Status new_status(s.error_code(),
                      ::base::StringPrintf("Failed to register node : %s",
                                           s.error_message().c_str()));
    node->OnError(new_status);
    return;
  }

  const NodeInfo& node_info = response->node_info();
  node->OnDidCreate(node_info);
  nodes_.push_back(std::move(node));
}

void MasterProxy::SubscribeTopicAsync(
    const SubscribeTopicRequest* request, SubscribeTopicResponse* response,
    StatusCallback callback, TopicInfoWatcher::NewTopicInfoCallback callback2) {
  topic_info_watcher_.RegisterCallback(request->topic(), callback2);
  master_client_interface_->SubscribeTopicAsync(request, response,
                                                std::move(callback));
}

void MasterProxy::UnsubscribeTopicAsync(const UnsubscribeTopicRequest* request,
                                        UnsubscribeTopicResponse* response,
                                        StatusCallback callback) {
  topic_info_watcher_.UnregisterCallback(request->topic());
  master_client_interface_->UnsubscribeTopicAsync(request, response,
                                                  std::move(callback));
}

}  // namespace felicia