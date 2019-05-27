#ifndef FELICIA_CORE_COMMUNICATION_DYNAMIC_SUBSCRIBIER_H_
#define FELICIA_CORE_COMMUNICATION_DYNAMIC_SUBSCRIBIER_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/protobuf_loader.h"

namespace felicia {

class DynamicSubscriber : public Subscriber<DynamicProtobufMessage> {
 public:
  DynamicSubscriber();
  ~DynamicSubscriber();

  void Subscribe(OnMessageCallback on_message_callback,
                 StatusCallback on_error_callback,
                 const communication::Settings& settings);

  void OnFindPublisher(const TopicInfo& topic_info);

  void Unsubscribe(const std::string& topic, StatusOnceCallback callback);

  const TopicInfo& topic_info() const { return topic_info_; }

 private:
  void ResetMessage(const TopicInfo& topic_info) override;

  DISALLOW_COPY_AND_ASSIGN(DynamicSubscriber);
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_DYNAMIC_SUBSCRIBIER_H_