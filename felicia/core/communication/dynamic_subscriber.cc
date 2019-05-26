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

  DCHECK(IsUnregistered()) << register_state_.ToString();

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

void DynamicSubscriber::Unsubscribe(const std::string& topic,
                                    StatusOnceCallback callback) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE,
        ::base::BindOnce(&DynamicSubscriber::Unsubscribe,
                         ::base::Unretained(this), topic, std::move(callback)));
    return;
  }

  // Unsubscribe function can be called either when topic info is updated to
  // UNREGISTERED state or manually unregistration from the js side. If both
  // cases happens almost same time, one of them should be ignored.
  if (IsUnregistered()) {
    DCHECK(IsStopping() || IsStopped()) << subscriber_state_.ToString();
    std::move(callback).Run(errors::Aborted("Already unsubscribed"));
    return;
  }

  DCHECK(IsRegistered()) << register_state_.ToString();

  register_state_.ToUnregistered();

  subscriber_state_.ToStopping();
  master_proxy.PostDelayedTask(
      FROM_HERE,
      ::base::BindOnce(&DynamicSubscriber::CheckIfStoppedAndCallback,
                       ::base::Unretained(this), std::move(callback)),
      settings_.period);
}

void DynamicSubscriber::CheckIfStoppedAndCallback(StatusOnceCallback callback) {
  if (IsStopped()) {
    std::move(callback).Run(Status::OK());
    return;
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostDelayedTask(
      FROM_HERE,
      ::base::BindOnce(&DynamicSubscriber::CheckIfStoppedAndCallback,
                       ::base::Unretained(this), std::move(callback)),
      settings_.period);
}

void DynamicSubscriber::ResetMessage(const TopicInfo& topic_info) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  message_.Reset(master_proxy.protobuf_loader()
                     ->NewMessage(topic_info.type_name())
                     ->New());
}

}  // namespace felicia