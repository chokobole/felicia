#include "felicia/core/communication/dynamic_subscriber.h"

namespace felicia {

DynamicSubscriber::DynamicSubscriber() = default;

DynamicSubscriber::~DynamicSubscriber() = default;

void DynamicSubscriber::Subscribe(OnMessageCallback on_message_callback,
                                  StatusCallback on_error_callback,
                                  const communication::Settings& settings) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE, ::base::BindOnce(
                       &DynamicSubscriber::Subscribe, ::base::Unretained(this),
                       on_message_callback, on_error_callback, settings));
    return;
  }

  DCHECK(register_state_.IsUnregistered());

  register_state_.ToRegistered();

  on_message_callback_ = on_message_callback;
  on_error_callback_ = on_error_callback;
  settings_ = settings;

  subscriber_state_.ToStopped();
}

void DynamicSubscriber::OnFindPublisher(const TopicInfo& topic_info) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE, ::base::BindOnce(&DynamicSubscriber::OnFindPublisher,
                                    ::base::Unretained(this), topic_info));
    return;
  }

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

  DCHECK(register_state_.IsRegistered());

  register_state_.ToUnregistered();

  subscriber_state_.ToStopping();
  master_proxy.PostDelayedTask(
      FROM_HERE,
      ::base::BindOnce(&DynamicSubscriber::Release, ::base::Unretained(this),
                       std::move(callback)),
      settings_.period * 2);
}

void DynamicSubscriber::Release(StatusOnceCallback callback) {
  Subscriber<DynamicProtobufMessage>::Release();
  std::move(callback).Run(Status::OK());
}

void DynamicSubscriber::ResetMessage(const TopicInfo& topic_info) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  message_.Reset(master_proxy.protobuf_loader()
                     ->NewMessage(topic_info.type_name())
                     ->New());
}

}  // namespace felicia