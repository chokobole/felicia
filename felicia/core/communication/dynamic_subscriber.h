#ifndef FELICIA_CORE_COMMUNICATION_DYNAMIC_SUBSCRIBIER_H_
#define FELICIA_CORE_COMMUNICATION_DYNAMIC_SUBSCRIBIER_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/protobuf_loader.h"

namespace felicia {

class DynamicSubscriber : private Subscriber<DynamicProtobufMessage> {
 public:
  void Subscribe(OnMessageCallback on_message_callback,
                 OnErrorCallback on_error_callback,
                 const communication::Settings& settings,
                 const TopicInfo& topic_info, ProtobufLoader* loader);

  void OnFindPublisher(const TopicInfo& topic_info);
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_DYNAMIC_SUBSCRIBIER_H_