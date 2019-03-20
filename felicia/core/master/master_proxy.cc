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
      topic_info_watcher_(),
      heart_beat_signaller_(this) {}

MasterProxy::~MasterProxy() = default;

MasterProxy& MasterProxy::GetInstance() {
  static ::base::NoDestructor<MasterProxy> master_proxy;
  return *master_proxy;
}

bool MasterProxy::IsBoundToCurrentThread() const {
  return message_loop_->IsBoundToCurrentThread();
}

// TaskRunnerInterface methods
bool MasterProxy::PostTask(const ::base::Location& from_here,
                           ::base::OnceClosure callback) {
  return message_loop_->task_runner()->PostTask(from_here, std::move(callback));
}

bool MasterProxy::PostDelayedTask(const ::base::Location& from_here,
                                  ::base::OnceClosure callback,
                                  ::base::TimeDelta delay) {
  return message_loop_->task_runner()->PostDelayedTask(
      from_here, std::move(callback), delay);
}

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

Status MasterProxy::Stop() {
  Status s = master_client_interface_->Stop();
  run_loop_->Quit();
  return s;
}

#define CLIENT_METHOD(method)                                     \
  void MasterProxy::method##Async(const method##Request* request, \
                                  method##Response* response,     \
                                  StatusCallback done) {          \
    master_client_interface_->method##Async(request, response,    \
                                            std::move(done));     \
  }

CLIENT_METHOD(RegisterClient)
CLIENT_METHOD(ListClients)
CLIENT_METHOD(RegisterNode)
CLIENT_METHOD(UnregisterNode)
CLIENT_METHOD(ListNodes)
CLIENT_METHOD(PublishTopic)
CLIENT_METHOD(UnpublishTopic)
CLIENT_METHOD(SubscribeTopic)
// UnsubscribeTopic needs additional remove callback from
// |topic_info_watcher_|
// CLIENT_METHOD(UnsubscribeTopic)
CLIENT_METHOD(ListTopics)

#undef CLIENT_METHOD

void MasterProxy::Run() { run_loop_->Run(); }

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