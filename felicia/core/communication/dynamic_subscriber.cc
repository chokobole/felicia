#include "felicia/core/communication/dynamic_subscriber.h"

namespace felicia {

DynamicSubscriber::DynamicSubscriber() = default;

DynamicSubscriber::~DynamicSubscriber() = default;

void DynamicSubscriber::Subscribe(const communication::Settings& settings,
                                  OnMessageCallback on_message_callback,
                                  StatusCallback on_error_callback) {
#if DCHECK_IS_ON()
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  DCHECK(master_proxy.IsBoundToCurrentThread());
#endif
  DLOG(INFO) << FROM_HERE.ToString();
  DCHECK(IsUnregistered()) << register_state_.ToString();

  register_state_.ToRegistered(FROM_HERE);

  channel_types_ = AllChannelTypes();
  on_message_callback_ = on_message_callback;
  on_error_callback_ = on_error_callback;
  settings_ = settings;

  subscriber_state_.ToStopped(FROM_HERE);
}

void DynamicSubscriber::OnFindPublisher(const TopicInfo& topic_info) {
#if DCHECK_IS_ON()
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  DCHECK(master_proxy.IsBoundToCurrentThread());
#endif
  DLOG(INFO) << FROM_HERE.ToString();
  Subscriber<DynamicProtobufMessage>::OnFindPublisher(topic_info);
}

void DynamicSubscriber::Unsubscribe(const std::string& topic,
                                    StatusOnceCallback callback) {
#if DCHECK_IS_ON()
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  DCHECK(master_proxy.IsBoundToCurrentThread());
#endif
  DLOG(INFO) << FROM_HERE.ToString();
  // Unsubscribe function can be called either when topic info is updated to
  // UNREGISTERED state or manually unregistration from the js side. If both
  // cases happens almost same time, one of them should be ignored.
  if (IsUnregistered()) {
    DCHECK(IsStopping() || IsStopped()) << subscriber_state_.ToString();
    std::move(callback).Run(errors::Aborted("Already unsubscribed"));
    return;
  }

  DCHECK(IsRegistered()) << register_state_.ToString();

  register_state_.ToUnregistered(FROM_HERE);

  StopMessageLoop(std::move(callback));
}

bool DynamicSubscriber::MaybeResolveMessgaeType(const TopicInfo& topic_info) {
  if (topic_info.impl_type() == TopicInfo::ROS) {
    LOG(ERROR) << "Can't subscribie dynamically for ROS message.";
    return false;
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  const google::protobuf::Message* message =
      master_proxy.protobuf_loader()->NewMessage(topic_info.type_name());
  if (!message) return false;
  message_.Reset(message->New());
  return true;
}

}  // namespace felicia