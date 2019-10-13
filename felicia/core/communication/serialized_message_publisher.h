#ifndef FELICIA_CORE_COMMUNICATION_SERIALIZED_MESSAGE_PUBLIHSER_H_
#define FELICIA_CORE_COMMUNICATION_SERIALIZED_MESSAGE_PUBLIHSER_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/message/serialized_message.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace felicia {

class SerializedMessagePublisher : public Publisher<SerializedMessage> {
 public:
  SerializedMessagePublisher();
  explicit SerializedMessagePublisher(
      const std::string& message_type_name,
      TopicInfo::ImplType impl_type = TopicInfo::PROTOBUF);
  ~SerializedMessagePublisher();

  void SetMessageTypeName(const std::string& message_type_name);
  std::string GetMessageTypeName() const override;
  void SetMessageImplType(TopicInfo::ImplType impl_type);
  TopicInfo::ImplType GetMessageImplType() const override;

  void PublishFromSerialized(
      const std::string& serialized,
      SendMessageCallback callback = SendMessageCallback());

  void PublishFromSerialized(
      std::string&& serialized,
      SendMessageCallback callback = SendMessageCallback());

 protected:
  MessageIOError SerializeToString(SerializedMessage* message,
                                   std::string* serialized) override;

  SerializedMessage message_;
  std::string message_type_name_;
  TopicInfo::ImplType impl_type_ = TopicInfo::PROTOBUF;

  DISALLOW_COPY_AND_ASSIGN(SerializedMessagePublisher);
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_SERIALIZED_MESSAGE_PUBLIHSER_H_