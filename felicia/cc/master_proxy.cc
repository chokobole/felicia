#include "felicia/cc/master_proxy.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/master/client/grpc_master_client.h"
#include "felicia/core/master/rpc/grpc_util.h"
#include "felicia/core/platform/net_util.h"

namespace felicia {

MasterProxy::MasterProxy()
    : message_loop_(
          std::make_unique<::base::MessageLoop>(::base::MessageLoop::TYPE_IO)),
      run_loop_(std::make_unique<::base::RunLoop>()),
      topic_info_watcher_(this),
      heart_beat_signaller_(this) {
  auto channel = ConnectGRPCService();
  master_client_interface_ = std::make_unique<GrpcMasterClient>(channel);
  master_client_interface_->Start();

  heart_beat_signaller_.Start();
  topic_info_watcher_.Start();

  *client_info_.mutable_heart_beat_signaller_source() =
      heart_beat_signaller_.channel_source();
  *client_info_.mutable_topic_watcher_source() =
      topic_info_watcher_.channel_source();

  RegisterClient();
}

void MasterProxy::Run() {
  run_loop_->Run();
  master_client_interface_->Join();
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

void MasterProxy::PublishTopicAsync(PublishTopicRequest* request,
                                    PublishTopicResponse* response,
                                    StatusCallback callback) {
  master_client_interface_->PublishTopicAsync(request, response,
                                              std::move(callback));
}

void MasterProxy::SubscribeTopicAsync(
    SubscribeTopicRequest* request, SubscribeTopicResponse* response,
    StatusCallback callback, TopicInfoWatcher::NewTopicInfoCallback callback2) {
  topic_info_watcher_.RegisterCallback(request->topic(), callback2);
  master_client_interface_->SubscribeTopicAsync(request, response,
                                                std::move(callback));
}

}  // namespace felicia