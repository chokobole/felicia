#ifndef FELICIA_CORE_COMMUNICATION_SERIALIZED_MESSAGE_SUBSCRIBIER_H_
#define FELICIA_CORE_COMMUNICATION_SERIALIZED_MESSAGE_SUBSCRIBIER_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/message/serialized_message.h"

namespace felicia {

class SerializedMessageSubscriber : public Subscriber<SerializedMessage> {
 public:
  SerializedMessageSubscriber();
  explicit SerializedMessageSubscriber(
      const std::string& message_type_name,
      TopicInfo::ImplType impl_type = TopicInfo::PROTOBUF);
  ~SerializedMessageSubscriber();

 protected:
#if defined(HAS_ROS)
  std::string GetMessageMD5Sum() const override;
#endif  // defined(HAS_ROS)
  std::string GetMessageTypeName() const override;

#if defined(HAS_ROS)
  std::string message_md5_sum_;
#endif  // defined(HAS_ROS)
  std::string message_type_name_;
  TopicInfo::ImplType impl_type_ = TopicInfo::PROTOBUF;

  DISALLOW_COPY_AND_ASSIGN(SerializedMessageSubscriber);
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_SERIALIZED_MESSAGE_SUBSCRIBIER_H_