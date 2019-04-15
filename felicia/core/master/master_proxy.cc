#include "felicia/core/master/master_proxy.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/lib/net/net_util.h"
#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/master/rpc/grpc_master_client.h"
#include "felicia/core/master/rpc/grpc_util.h"

namespace felicia {

namespace {

bool g_on_background = false;

}  // namespace

MasterProxy::MasterProxy()
    : topic_info_watcher_(this), heart_beat_signaller_(this) {
  if (g_on_background) {
    thread_ = std::make_unique<::base::Thread>("MasterProxy");
  } else {
    message_loop_ =
        std::make_unique<::base::MessageLoop>(::base::MessageLoop::TYPE_IO);
    run_loop_ = std::make_unique<::base::RunLoop>();
  }
}

MasterProxy::~MasterProxy() = default;

// static
void MasterProxy::SetBackground() { g_on_background = true; }

// static
MasterProxy& MasterProxy::GetInstance() {
  static ::base::NoDestructor<MasterProxy> master_proxy;
  return *master_proxy;
}

bool MasterProxy::IsBoundToCurrentThread() const {
  if (g_on_background) {
    return thread_->task_runner()->BelongsToCurrentThread();
  } else {
    return message_loop_->IsBoundToCurrentThread();
  }
}

// TaskRunnerInterface methods
bool MasterProxy::PostTask(const ::base::Location& from_here,
                           ::base::OnceClosure callback) {
  if (g_on_background) {
    return thread_->task_runner()->PostTask(from_here, std::move(callback));
  } else {
    return message_loop_->task_runner()->PostTask(from_here,
                                                  std::move(callback));
  }
}

bool MasterProxy::PostDelayedTask(const ::base::Location& from_here,
                                  ::base::OnceClosure callback,
                                  ::base::TimeDelta delay) {
  if (g_on_background) {
    return thread_->task_runner()->PostDelayedTask(from_here,
                                                   std::move(callback), delay);
  } else {
    return message_loop_->task_runner()->PostDelayedTask(
        from_here, std::move(callback), delay);
  }
}

Status MasterProxy::Start() {
  auto channel = ConnectGRPCService();
  master_client_interface_ = std::make_unique<GrpcMasterClient>(channel);
  Status s = master_client_interface_->Start();
  if (!s.ok()) return s;

  if (g_on_background) {
    thread_->StartWithOptions(
        ::base::Thread::Options{::base::MessageLoop::TYPE_IO, 0});
  }

  ::base::WaitableEvent* event = new ::base::WaitableEvent;
  Setup(event);

  event->Wait();
  delete event;

  RegisterClient();

  return Status::OK();
}

Status MasterProxy::Stop() {
  Status s = master_client_interface_->Stop();
  if (g_on_background) {
    thread_->Stop();
  } else {
    run_loop_->Quit();
  }
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

void MasterProxy::Run() {
  if (g_on_background) return;
  run_loop_->Run();
}

void MasterProxy::Setup(::base::WaitableEvent* event) {
  if (!IsBoundToCurrentThread()) {
    PostTask(FROM_HERE, ::base::BindOnce(&MasterProxy::Setup,
                                         ::base::Unretained(this), event));
    return;
  }

  heart_beat_signaller_.Start();
  topic_info_watcher_.Start();

  *client_info_.mutable_heart_beat_signaller_source() =
      heart_beat_signaller_.channel_source();
  *client_info_.mutable_topic_info_watcher_source() =
      topic_info_watcher_.channel_source();

  event->Signal();
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