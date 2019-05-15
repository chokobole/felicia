#include "felicia/core/communication/dynamic_subscriber.h"

namespace felicia {

DynamicSubscriber::DynamicSubscriber() = default;

DynamicSubscriber::~DynamicSubscriber() = default;

void DynamicSubscriber::Subscribe(OnMessageCallback on_message_callback,
                                  StatusCallback on_error_callback,
                                  const communication::Settings& settings) {
  on_message_callback_ = on_message_callback;
  on_error_callback_ = on_error_callback;
  settings_ = settings;

  state_.ToStopped();
}

void DynamicSubscriber::OnFindPublisher(const TopicInfo& topic_info) {
  Subscriber<DynamicProtobufMessage>::OnFindPublisher(topic_info);
}

void DynamicSubscriber::UnSubscribe(const std::string& topic,
                                    StatusOnceCallback callback) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE,
        ::base::BindOnce(&DynamicSubscriber::UnSubscribe,
                         ::base::Unretained(this), topic, std::move(callback)));
    return;
  }

  on_message_callback_.Reset();
  on_error_callback_.Reset();

  channel_.reset();
  message_queue_.clear();

  state_.ToStopped();
  std::move(callback).Run(Status::OK());
}

void DynamicSubscriber::ResetMessage(const TopicInfo& topic_info) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  message_.Reset(master_proxy.protobuf_loader()
                     ->NewMessage(topic_info.type_name())
                     ->New());
}

}  // namespace felicia