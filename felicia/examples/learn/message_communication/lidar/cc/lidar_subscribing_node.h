#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_LIDAR_SUBSCRIBING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_LIDAR_SUBSCRIBING_NODE_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/lidar/lidar_frame_message.pb.h"

namespace felicia {

class LidarSubscribingNode : public NodeLifecycle {
 public:
  LidarSubscribingNode(const std::string& topic) : topic_(topic) {}

  void OnInit() override {
    std::cout << "LidarSubscribingNode::OnInit()" << std::endl;
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "LidarSubscribingNode::OnDidCreate()" << std::endl;
    node_info_ = node_info;
    RequestSubscribe();
  }

  void OnError(const Status& s) override {
    std::cout << "LidarSubscribingNode::OnError()" << std::endl;
    LOG(ERROR) << s.error_message();
  }

  void RequestSubscribe() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.period = ::base::TimeDelta::FromMilliseconds(100);
    settings.is_dynamic_buffer = true;

    subscriber_.RequestSubscribe(
        node_info_, topic_, ChannelDef::TCP | ChannelDef::UDP,
        ::base::BindRepeating(&LidarSubscribingNode::OnMessage,
                              ::base::Unretained(this)),
        ::base::BindRepeating(&LidarSubscribingNode::OnSubscriptionError,
                              ::base::Unretained(this)),
        settings,
        ::base::BindOnce(&LidarSubscribingNode::OnRequestSubscribe,
                         ::base::Unretained(this)));
  }

  void OnMessage(LidarFrameMessage&& message) {
    std::cout << "LidarSubscribingNode::OnMessage()" << std::endl;
  }

  void OnSubscriptionError(const Status& s) {
    std::cout << "LidarSubscribingNode::OnSubscriptionError()" << std::endl;
    LOG(ERROR) << s.error_message();
  }

  void OnRequestSubscribe(const Status& s) {
    std::cout << "LidarSubscribingNode::OnRequestSubscribe()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void RequestUnsubscribe() {
    subscriber_.RequestUnsubscribe(
        node_info_, topic_,
        ::base::BindOnce(&LidarSubscribingNode::OnRequestUnsubscribe,
                         ::base::Unretained(this)));
  }

  void OnRequestUnsubscribe(const Status& s) {
    std::cout << "LidarSubscribingNode::OnRequestUnsubscribe()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

 private:
  NodeInfo node_info_;
  std::string topic_;
  Subscriber<LidarFrameMessage> subscriber_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_LIDAR_SUBSCRIBING_NODE_H_