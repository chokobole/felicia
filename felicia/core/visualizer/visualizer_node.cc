#include "felicia/core/visualizer/visualizer_node.h"

#include "third_party/chromium/base/bind.h"

#include "felicia/core/lib/felicia_env.h"
#include "felicia/core/master/master_proxy.h"

namespace felicia {

VisualizerNode::VisualizerNode() = default;
VisualizerNode::~VisualizerNode() = default;

void VisualizerNode::OnInit() {
  loader_ = ProtobufLoader::Load(::base::FilePath(FELICIA_ROOT));

  MasterProxy& master_proxy = MasterProxy::GetInstance();

  master_proxy.topic_info_watcher_.RegisterAllTopicCallback(
      ::base::BindRepeating(&VisualizerNode::OnFindPublisher,
                            ::base::Unretained(this)));
}

void VisualizerNode::OnDidCreate(const NodeInfo& node_info) {}

void VisualizerNode::OnError(const Status& s) {
  if (!s.ok()) {
    LOG(ERROR) << s.error_message();
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.Stop();
  }
}

void VisualizerNode::OnFindPublisher(const TopicInfo& topic_info) {
  LOG(INFO) << topic_info.DebugString();
  std::unique_ptr<DynamicSubscriber> subscriber =
      std::make_unique<DynamicSubscriber>();

  communication::Settings settings;
  subscriber->Subscribe(
      ::base::BindRepeating(&VisualizerNode::OnNewMessage,
                            ::base::Unretained(this), topic_info.topic()),
      ::base::BindRepeating(&VisualizerNode::OnSubscriptionError,
                            ::base::Unretained(this), topic_info.topic()),
      settings, topic_info, loader_.get());
  subscriber->OnFindPublisher(topic_info);
  subscribers_.push_back(std::move(subscriber));
}

void VisualizerNode::OnNewMessage(const std::string& topic,
                                  const DynamicProtobufMessage& message) {
  LOG(INFO) << "topic: " << topic;

  LOG(INFO) << message.DebugString();
}

void VisualizerNode::OnSubscriptionError(const std::string& topic,
                                         const Status& s) {
  LOG(ERROR) << "topic: " << topic;
  LOG(ERROR) << s.error_message();
}

}  // namespace felicia