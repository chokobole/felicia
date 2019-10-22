#include "felicia/core/node/dynamic_publishing_node.h"

namespace felicia {

DynamicPublishingNode::DynamicPublishingNode(std::unique_ptr<Delegate> delegate)
    : delegate_(std::move(delegate)) {}

DynamicPublishingNode::~DynamicPublishingNode() = default;

void DynamicPublishingNode::OnDidCreate(NodeInfo node_info) {
  node_info_ = std::move(node_info);
  delegate_->OnDidCreate(this);
}

void DynamicPublishingNode::OnError(Status s) { LOG(ERROR) << s; }

void DynamicPublishingNode::RequestPublish(
    const std::string& message_type, const std::string& topic, int channel_defs,
    const communication::Settings& settings) {
  publisher_ = std::make_unique<DynamicPublisher>();
  if (!publisher_->ResolveType(message_type)) return;

  publisher_->RequestPublish(
      node_info_, topic, channel_defs, settings,
      base::BindOnce(&DynamicPublishingNode::OnRequestPublish,
                     base::Unretained(this)));
}

void DynamicPublishingNode::RequestUnpublish(const std::string& topic) {
  publisher_->RequestUnpublish(
      node_info_, topic,
      base::BindOnce(&DynamicPublishingNode::OnRequestUnpublish,
                     base::Unretained(this)));
}

void DynamicPublishingNode::PublishMessageFromJson(
    const std::string& json_message) {
  publisher_->PublishFromJson(
      json_message,
      base::BindRepeating(&DynamicPublishingNode::Delegate::OnPublish,
                          base::Unretained(delegate_.get())));
}

void DynamicPublishingNode::OnRequestPublish(Status s) {
  if (!s.ok()) {
    publisher_.reset();
  }
  delegate_->OnRequestPublish(std::move(s));
}

void DynamicPublishingNode::OnRequestUnpublish(Status s) {
  if (s.ok()) {
    publisher_.reset();
  }
  delegate_->OnRequestUnpublish(std::move(s));
}

}  // namespace felicia