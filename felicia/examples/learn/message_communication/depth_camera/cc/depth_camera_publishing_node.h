#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_DEPTH_CAMERA_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_DEPTH_CAMERA_PUBLISHING_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/camera/depth_camera_interface.h"

namespace felicia {

extern std::unique_ptr<DepthCameraInterface> MakeNewDepthCamera(
    const CameraDescriptor& camera_descriptor);

class DepthCameraPublishingNode : public NodeLifecycle {
 public:
  DepthCameraPublishingNode(const std::string& topic,
                            const std::string& channel_type,
                            const CameraDescriptor& camera_descriptor,
                            size_t buffer_size)
      : topic_(topic),
        camera_descriptor_(camera_descriptor),
        buffer_size_(buffer_size) {
    ChannelDef_Type type;
    ChannelDef_Type_Parse(channel_type, &type);
    channel_def_.set_type(type);
    std::cout << "DepthCameraPublishingNode()" << std::endl;
  }

  void OnInit() override {
    std::cout << "DepthCameraPublishingNode::OnInit()" << std::endl;
    camera_ = MakeNewDepthCamera(camera_descriptor_);
    CHECK(camera_->Init().ok());
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "DepthCameraPublishingNode::OnDidCreate()" << std::endl;
    node_info_ = node_info;
    RequestPublish();
  }

  void OnError(const Status& s) override {
    std::cout << "DepthCameraPublishingNode::OnError()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void RequestPublish() {
    communication::Settings settings;
    settings.buffer_size = Bytes::FromBytes(buffer_size_);

    publisher_.RequestPublish(
        node_info_, topic_, channel_def_, settings,
        ::base::BindOnce(&DepthCameraPublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));
  }

  void OnRequestPublish(const Status& s) {
    std::cout << "DepthCameraPublishingNode::OnRequestPublish()" << std::endl;
    if (s.ok()) {
      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(
          FROM_HERE, ::base::BindOnce(&DepthCameraPublishingNode::StartCamera,
                                      ::base::Unretained(this)));
    } else {
      LOG(ERROR) << s.error_message();
    }
  }

  void StartCamera() {
    Status s = camera_->Start(
        CameraFormat(640, 480, PIXEL_FORMAT_YUY2, 5),
        CameraFormat(640, 480, PIXEL_FORMAT_Z16, 5),
        ::base::BindRepeating(&DepthCameraPublishingNode::OnColorFrame,
                              ::base::Unretained(this)),
        ::base::BindRepeating(&DepthCameraPublishingNode::OnDepthFrame,
                              ::base::Unretained(this)),
        ::base::BindRepeating(&DepthCameraPublishingNode::OnCameraError,
                              ::base::Unretained(this)));
    if (s.ok()) {
      // MasterProxy& master_proxy = MasterProxy::GetInstance();
      // master_proxy.PostDelayedTask(
      //     FROM_HERE,
      //     ::base::BindOnce(&DepthCameraPublishingNode::RequestUnpublish,
      //                      ::base::Unretained(this)),
      //     ::base::TimeDelta::FromSeconds(10));
    } else {
      LOG(ERROR) << s.error_message();
    }
  }

  void OnColorFrame(CameraFrame camera_frame) {
    std::cout << "DepthCameraPublishingNode::OnColorFrame" << std::endl;
    if (publisher_.IsUnregistered()) return;

    publisher_.Publish(camera_frame.ToCameraFrameMessage(),
                       ::base::BindOnce(&DepthCameraPublishingNode::OnPublish,
                                        ::base::Unretained(this)));
  }

  void OnDepthFrame(CameraFrame camera_frame) {
    std::cout << "DepthCameraPublishingNode::OnDepthFrame" << std::endl;
    if (publisher_.IsUnregistered()) return;

    publisher_.Publish(camera_frame.ToCameraFrameMessage(),
                       ::base::BindOnce(&DepthCameraPublishingNode::OnPublish,
                                        ::base::Unretained(this)));
  }

  void OnCameraError(const Status& s) {
    std::cout << "DepthCameraPublishingNode::OnCameraError" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void OnPublish(const Status& s) {
    std::cout << "DepthCameraPublishingNode::OnPublish()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void RequestUnpublish() {
    publisher_.RequestUnpublish(
        node_info_, topic_,
        ::base::BindOnce(&DepthCameraPublishingNode::OnRequestUnpublish,
                         ::base::Unretained(this)));
  }

  void OnRequestUnpublish(const Status& s) {
    std::cout << "DepthCameraPublishingNode::OnRequestUnpublish()" << std::endl;
    if (s.ok()) {
      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(
          FROM_HERE, ::base::BindOnce(&DepthCameraPublishingNode::StopCamera,
                                      ::base::Unretained(this)));
    } else {
      LOG(ERROR) << s.error_message();
    }
  }

  void StopCamera() {
    Status s = camera_->Stop();
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

 private:
  NodeInfo node_info_;
  std::string topic_;
  ChannelDef channel_def_;
  CameraDescriptor camera_descriptor_;
  size_t buffer_size_;
  Publisher<CameraFrameMessage> publisher_;
  std::unique_ptr<DepthCameraInterface> camera_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_DEPTH_CAMERA_PUBLISHING_NODE_H_