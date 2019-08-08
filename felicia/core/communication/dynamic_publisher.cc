#include "felicia/core/communication/dynamic_publisher.h"

#include "google/protobuf/util/json_util.h"

namespace felicia {

DynamicPublisher::DynamicPublisher() = default;

DynamicPublisher::~DynamicPublisher() = default;

void DynamicPublisher::ResetMessage(const std::string& message_type) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  message_prototype_.Reset(
      master_proxy.protobuf_loader()->NewMessage(message_type)->New());
}

void DynamicPublisher::PublishFromJson(const std::string& json_message,
                                       SendMessageCallback callback) {
  DCHECK(message_prototype_.message());

  google::protobuf::util::Status status =
      google::protobuf::util::JsonStringToMessage(json_message,
                                                  message_prototype_.message());
  if (!status.ok()) {
    callback.Run(ChannelDef::CHANNEL_TYPE_NONE,
                 Status(static_cast<felicia::error::Code>(status.error_code()),
                        status.error_message().ToString()));
    return;
  }

  Publisher<DynamicProtobufMessage>::Publish(message_prototype_, callback);
}

void DynamicPublisher::PublishFromSerialized(const std::string& serialized,
                                             SendMessageCallback callback) {
  DCHECK(message_prototype_.message());

  if (!message_prototype_.ParseFromArray(serialized.data(),
                                         serialized.length())) {
    callback.Run(ChannelDef::CHANNEL_TYPE_NONE,
                 errors::InvalidArgument(MessageIoErrorToString(
                     MessageIoError::ERR_FAILED_TO_PARSE)));
    return;
  }

  Publisher<DynamicProtobufMessage>::Publish(message_prototype_, callback);
}

std::string DynamicPublisher::GetMessageTypeName() const {
  return message_prototype_.GetTypeName();
}

}  // namespace felicia