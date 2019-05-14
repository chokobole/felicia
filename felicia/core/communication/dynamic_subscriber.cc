#include "felicia/core/communication/dynamic_subscriber.h"

namespace felicia {

DynamicSubscriber::DynamicSubscriber(ProtobufLoader* loader)
    : loader_(loader) {}

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

void DynamicSubscriber::ResetMessage(const TopicInfo& topic_info) {
  message_.Reset(loader_->NewMessage(topic_info.type_name())->New());
}

}  // namespace felicia