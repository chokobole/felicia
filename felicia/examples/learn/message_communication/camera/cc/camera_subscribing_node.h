#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_SUBSCRIBING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_SUBSCRIBING_NODE_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/camera/camera_frame_message.pb.h"
#include "felicia/examples/learn/message_communication/common/cc/camera_flag.h"

namespace felicia {

class CameraSubscribingNode : public NodeLifecycle {
 public:
  explicit CameraSubscribingNode(const CameraFlag& camera_flag)
      : camera_flag_(camera_flag), topic_(camera_flag_.topic_flag()->value()) {}

  void OnInit() override {
    std::cout << "CameraSubscribingNode::OnInit()" << std::endl;
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "CameraSubscribingNode::OnDidCreate()" << std::endl;
    node_info_ = node_info;
    RequestSubscribe();
  }

  void OnError(const Status& s) override {
    std::cout << "CameraSubscribingNode::OnError()" << std::endl;
    LOG(ERROR) << s;
  }

  void RequestSubscribe() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.period =
        base::TimeDelta::FromSecondsD(1.0 / camera_flag_.fps_flag()->value());
    settings.is_dynamic_buffer = true;

    subscriber_.RequestSubscribe(
        node_info_, topic_,
        ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_SHM,
        base::BindRepeating(&CameraSubscribingNode::OnMessage,
                            base::Unretained(this)),
        base::BindRepeating(&CameraSubscribingNode::OnSubscriptionError,
                            base::Unretained(this)),
        settings,
        base::BindOnce(&CameraSubscribingNode::OnRequestSubscribe,
                       base::Unretained(this)));
  }

  void OnMessage(drivers::CameraFrameMessage&& message) {
    std::cout << "CameraSubscribingNode::OnMessage()" << std::endl;

    // static int frame_number = 0;
    // char filename[15];
    // frame_number++;
    // sprintf(filename, "frame-%d.argb", frame_number);
    // FILE* fp = fopen(filename, "wb");

    // fwrite(message.data().c_str(), message.data().size(), 1, fp);

    // fflush(fp);
    // fclose(fp);
  }

  void OnSubscriptionError(const Status& s) {
    std::cout << "CameraSubscribingNode::OnSubscriptionError()" << std::endl;
    LOG(ERROR) << s;
  }

  void OnRequestSubscribe(const Status& s) {
    std::cout << "CameraSubscribingNode::OnRequestSubscribe()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s;
  }

  void RequestUnsubscribe() {
    subscriber_.RequestUnsubscribe(
        node_info_, topic_,
        base::BindOnce(&CameraSubscribingNode::OnRequestUnsubscribe,
                       base::Unretained(this)));
  }

  void OnRequestUnsubscribe(const Status& s) {
    std::cout << "CameraSubscribingNode::OnRequestUnsubscribe()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s;
  }

 private:
  NodeInfo node_info_;
  const CameraFlag& camera_flag_;
  const std::string topic_;
  Subscriber<drivers::CameraFrameMessage> subscriber_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_SUBSCRIBING_NODE_H_