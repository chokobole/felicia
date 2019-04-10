#include "felicia/core/communication/dynamic_subscriber.h"

namespace felicia {

void DynamicSubscriber::Subscribe(OnMessageCallback on_message_callback,
                                  OnErrorCallback on_error_callback,
                                  const communication::Settings& settings,
                                  const TopicInfo& topic_info,
                                  ProtobufLoader* loader) {
  on_message_callback_ = on_message_callback;
  on_error_callback_ = on_error_callback;
  queue_size_ = settings.queue_size;
  period_ = ::base::TimeDelta::FromMilliseconds(settings.period);
  message_.Reset(loader->NewMessage(topic_info.type_name())->New());

  state_.ToStopped();
}

void DynamicSubscriber::OnFindPublisher(const TopicInfo& topic_info) {
  Subscriber<DynamicProtobufMessage>::OnFindPublisher(topic_info);
}

}  // namespace felicia