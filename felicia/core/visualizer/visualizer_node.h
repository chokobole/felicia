#ifndef FELICIA_CORE_VISUALIZER_VISUALIZER_NODE_H_
#define FELICIA_CORE_VISUALIZER_VISUALIZER_NODE_H_

#include <memory>

#include "felicia/core/communication/dynamic_subscriber.h"
#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/protobuf_loader.h"
#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

class VisualizerNode : public NodeLifecycle {
 public:
  VisualizerNode();
  ~VisualizerNode();

  void OnInit() override;

  void OnDidCreate(const NodeInfo& node_info) override;

  void OnError(const Status& s) override;

 private:
  void OnFindPublisher(const TopicInfo& topic_info);

  void OnNewMessage(const std::string& topic,
                    const DynamicProtobufMessage& message);

  void OnSubscriptionError(const std::string& topic, const Status& s);

  std::unique_ptr<ProtobufLoader> loader_;
  std::vector<std::unique_ptr<DynamicSubscriber>> subscribers_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_VISUALIZER_VISUALIZER_NODE_H_