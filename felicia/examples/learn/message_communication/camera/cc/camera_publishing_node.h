#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_PUBLISHING_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/camera/camera_factory.h"
#include "felicia/examples/learn/message_communication/camera/camera_message.pb.h"

namespace felicia {

class CameraPublishingNode : public NodeLifecycle {
 public:
  CameraPublishingNode(const std::string& topic,
                       const std::string& channel_type,
                       const std::string& device_id, size_t buffer_size)
      : topic_(topic), device_id_(device_id), buffer_size_(buffer_size) {
    if (channel_type.compare("TCP") == 0) {
      channel_def_.set_type(ChannelDef::TCP);
    } else if (channel_type.compare("UDP") == 0) {
      channel_def_.set_type(ChannelDef::UDP);
    }
  }

  void OnInit() override {
    std::cout << "CameraPublishingNode::OnInit()" << std::endl;
    CameraDescriptor descriptor("simple camera", device_id_);
    camera_ = CameraFactory::NewCamera(descriptor);
    CHECK(camera_->Init().ok());
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "CameraPublishingNode::OnDidCreate()" << std::endl;
    node_info_ = node_info;
    RequestPublish();
  }

  void OnError(const Status& s) override {
    std::cout << "CameraPublishingNode::OnError()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void RequestPublish() {
    communication::Settings settings;
    settings.buffer_size = Bytes::FromBytes(buffer_size_);

    publisher_.RequestPublish(
        node_info_, topic_, channel_def_, settings,
        ::base::BindOnce(&CameraPublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));
  }

  void OnRequestPublish(const Status& s) {
    std::cout << "CameraPublishingNode::OnRequestPublish()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostTask(FROM_HERE,
                          ::base::BindOnce(&CameraPublishingNode::StartCamera,
                                           ::base::Unretained(this)));
  }

  void StartCamera() {
    camera_->Start(::base::BindRepeating(&CameraPublishingNode::OnCameraFrame,
                                         ::base::Unretained(this)),
                   ::base::BindRepeating(&CameraPublishingNode::OnCameraError,
                                         ::base::Unretained(this)));
  }

  void OnCameraFrame(CameraFrame camera_frame) {
    LOG(INFO) << "CameraPublishingNode::OnCameraFrame" << std::endl;
    if (last_timestamp_.is_zero()) {
      last_timestamp_ = camera_frame.timestamp();
    } else {
      if (camera_frame.timestamp() - last_timestamp_ <
          ::base::TimeDelta::FromMilliseconds(100)) {
        return;
      } else {
        last_timestamp_ = camera_frame.timestamp();
      }
    }

    CameraMessage message;
    message.set_data(camera_frame.data_ptr(), camera_frame.size());
    message.set_timestamp(camera_frame.timestamp().InSecondsF());
    publisher_.Publish(std::move(message),
                       ::base::BindOnce(&CameraPublishingNode::OnPublish,
                                        ::base::Unretained(this)));
  }

  void OnCameraError(const Status& s) {
    LOG(INFO) << "CameraPublishingNode::OnCameraError" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void OnPublish(const Status& s) {
    std::cout << "CameraPublishingNode::OnPublish()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

 private:
  NodeInfo node_info_;
  std::string topic_;
  ChannelDef channel_def_;
  std::string device_id_;
  size_t buffer_size_;
  Publisher<CameraMessage> publisher_;
  std::unique_ptr<CameraInterface> camera_;
  ::base::TimeDelta last_timestamp_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_PUBLISHING_NODE_H_