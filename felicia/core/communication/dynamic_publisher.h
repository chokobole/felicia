#ifndef FELICIA_CORE_COMMUNICATION_DYNAMIC_PUBLIHSER_H_
#define FELICIA_CORE_COMMUNICATION_DYNAMIC_PUBLIHSER_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/protobuf_loader.h"

namespace felicia {

class DynamicPublisher : public Publisher<DynamicProtobufMessage> {
 public:
  explicit DynamicPublisher(ProtobufLoader* loader);
  ~DynamicPublisher();

  void ResetMessage(const std::string& message_type);

  void Publish(const std::string& json_message, StatusCallback callback);

 private:
  std::string GetMessageTypeName() const override;

  ProtobufLoader* loader_;  // not owned;

  DynamicProtobufMessage message_prototype_;

  DISALLOW_COPY_AND_ASSIGN(DynamicPublisher);
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_DYNAMIC_PUBLIHSER_H_