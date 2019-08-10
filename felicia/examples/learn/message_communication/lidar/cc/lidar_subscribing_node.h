#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_LIDAR_SUBSCRIBING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_LIDAR_SUBSCRIBING_NODE_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/lidar/lidar_frame_message.pb.h"
#include "felicia/examples/learn/message_communication/lidar/cc/lidar_flag.h"

namespace felicia {

class LidarSubscribingNode : public NodeLifecycle {
 public:
  LidarSubscribingNode(const LidarFlag& lidar_flag)
      : lidar_flag_(lidar_flag), topic_(lidar_flag_.topic_flag()->value()) {}

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
    LOG(ERROR) << s;
  }

  void RequestSubscribe() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.period = base::TimeDelta::FromMilliseconds(100);
    settings.is_dynamic_buffer = true;

    subscriber_.RequestSubscribe(
        node_info_, topic_,
        ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_UDP |
            ChannelDef::CHANNEL_TYPE_SHM,
        base::BindRepeating(&LidarSubscribingNode::OnMessage,
                            base::Unretained(this)),
        base::BindRepeating(&LidarSubscribingNode::OnSubscriptionError,
                            base::Unretained(this)),
        settings,
        base::BindOnce(&LidarSubscribingNode::OnRequestSubscribe,
                       base::Unretained(this)));
  }

  void OnMessage(drivers::LidarFrameMessage&& message) {
    std::cout << "LidarSubscribingNode::OnMessage()" << std::endl;
  }

  void OnSubscriptionError(const Status& s) {
    std::cout << "LidarSubscribingNode::OnSubscriptionError()" << std::endl;
    LOG(ERROR) << s;
  }

  void OnRequestSubscribe(const Status& s) {
    std::cout << "LidarSubscribingNode::OnRequestSubscribe()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s;
  }

  void RequestUnsubscribe() {
    subscriber_.RequestUnsubscribe(
        node_info_, topic_,
        base::BindOnce(&LidarSubscribingNode::OnRequestUnsubscribe,
                       base::Unretained(this)));
  }

  void OnRequestUnsubscribe(const Status& s) {
    std::cout << "LidarSubscribingNode::OnRequestUnsubscribe()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s;
  }

 private:
  NodeInfo node_info_;
  const LidarFlag& lidar_flag_;
  const std::string topic_;
  Subscriber<drivers::LidarFrameMessage> subscriber_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_LIDAR_SUBSCRIBING_NODE_H_