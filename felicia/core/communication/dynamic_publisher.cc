#include "felicia/core/communication/dynamic_publisher.h"

#include "google/protobuf/util/json_util.h"

namespace felicia {

DynamicPublisher::DynamicPublisher(ProtobufLoader* loader) : loader_(loader) {}

DynamicPublisher::~DynamicPublisher() = default;

void DynamicPublisher::ResetMessage(const std::string& message_type) {
  message_prototype_.Reset(loader_->NewMessage(message_type)->New());
}

void DynamicPublisher::Publish(const std::string& json_message,
                               StatusOnceCallback callback) {
  DCHECK(message_prototype_.message());

  ::google::protobuf::util::Status status =
      ::google::protobuf::util::JsonStringToMessage(
          json_message, message_prototype_.message());
  if (!status.ok()) {
    std::move(callback).Run(
        Status(static_cast<felicia::error::Code>(status.error_code()),
               status.error_message().ToString()));
    return;
  }

  Publisher<DynamicProtobufMessage>::Publish(message_prototype_,
                                             std::move(callback));
}

std::string DynamicPublisher::GetMessageTypeName() const {
  return message_prototype_.GetTypeName();
}

}  // namespace felicia