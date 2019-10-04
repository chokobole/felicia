#include "felicia/core/communication/dynamic_publisher.h"

#include "google/protobuf/util/json_util.h"

namespace felicia {

DynamicPublisher::DynamicPublisher() = default;

DynamicPublisher::~DynamicPublisher() = default;

bool DynamicPublisher::ResolveType(const std::string& message_type) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  const google::protobuf::Message* message =
      master_proxy.protobuf_loader()->NewMessage(message_type);
  if (!message) return false;
  message_prototype_.Reset(message->New());
  return true;
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

std::string DynamicPublisher::GetMessageTypeName() const {
  return message_prototype_.GetTypeName();
}

}  // namespace felicia