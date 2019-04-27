#include "felicia/core/node/dynamic_publishing_node.h"

namespace felicia {

DynamicPublishingNode::DynamicPublishingNode(
    ProtobufLoader* loader, const std::string& topic,
    const std::string& message_type, const ChannelDef& channel_def,
    PublisherCallback publisher_callback)
    : loader_(loader),
      topic_(topic),
      message_type_(message_type),
      channel_def_(channel_def),
      publisher_callback_(std::move(publisher_callback)) {}

DynamicPublishingNode::~DynamicPublishingNode() = default;

void DynamicPublishingNode::OnInit() {}

void DynamicPublishingNode::OnDidCreate(const NodeInfo& node_info) {
  node_info_ = node_info;
  RequestPublish();
}

void DynamicPublishingNode::OnError(const Status& s) {
  LOG_IF(ERROR, !s.ok()) << s.error_message();
}

void DynamicPublishingNode::PublishMessageFromJSON(
    const std::string& json_message, StatusOnceCallback callback) {
  publisher_->Publish(json_message, std::move(callback));
}

void DynamicPublishingNode::RequestPublish() {
  publisher_ = std::make_unique<DynamicPublisher>(loader_);
  LOG(INFO) << message_type_;
  publisher_->ResetMessage(message_type_);

  communication::Settings settings;
  settings.is_dynamic_buffer = true;

  publisher_->RequestPublish(
      node_info_, topic_, channel_def_, settings,
      ::base::BindOnce(&DynamicPublishingNode::OnRequestPublish,
                       ::base::Unretained(this)));
}

void DynamicPublishingNode::OnRequestPublish(const Status& s) {
  if (s.ok()) {
    std::move(publisher_callback_).Run(publisher_.get());
  } else {
    std::move(publisher_callback_).Run(nullptr);
  }
}

}  // namespace felicia