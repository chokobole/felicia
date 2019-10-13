#ifndef FELICIA_CORE_COMMUNICATION_SERIALIZED_MESSAGE_SUBSCRIBIER_H_
#define FELICIA_CORE_COMMUNICATION_SERIALIZED_MESSAGE_SUBSCRIBIER_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/message/serialized_message.h"

namespace felicia {

class SerializedMessageSubscriber : public Subscriber<SerializedMessage> {
 public:
  explicit SerializedMessageSubscriber(
      TopicInfo::ImplType impl_type = TopicInfo::PROTOBUF);
  ~SerializedMessageSubscriber();

  std::string GetMessageTypeName() const;
  void SetMessageImplType(TopicInfo::ImplType impl_type);
  TopicInfo::ImplType GetMessageImplType() const;

 protected:
  bool MaybeResolveMessgaeType(const TopicInfo& topic_info) override;

  std::string message_type_name_;
  TopicInfo::ImplType impl_type_ = TopicInfo::PROTOBUF;

  DISALLOW_COPY_AND_ASSIGN(SerializedMessageSubscriber);
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_SERIALIZED_MESSAGE_SUBSCRIBIER_H_