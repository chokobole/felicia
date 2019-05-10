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
  DepthCameraPublishingNode(const std::string& color_topic,
                            const std::string& depth_topic,
                            const CameraDescriptor& camera_descriptor,
                            bool synched)
      : color_topic_(color_topic),
        depth_topic_(depth_topic),
        camera_descriptor_(camera_descriptor),
        synched_(synched) {}

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
    ChannelDef channel_def;

    communication::Settings settings;
    settings.is_dynamic_buffer = true;

    color_publisher_.RequestPublish(
        node_info_, color_topic_, channel_def, settings,
        ::base::BindOnce(&DepthCameraPublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));

    depth_publisher_.RequestPublish(
        node_info_, depth_topic_, channel_def, settings,
        ::base::BindOnce(&DepthCameraPublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));
  }

  void OnRequestPublish(const Status& s) {
    std::cout << "DepthCameraPublishingNode::OnRequestPublish()" << std::endl;
    if (s.ok()) {
      if (!(color_publisher_.IsRegistered() && depth_publisher_.IsRegistered()))
        return;

      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(
          FROM_HERE, ::base::BindOnce(&DepthCameraPublishingNode::StartCamera,
                                      ::base::Unretained(this)));
    } else {
      LOG(ERROR) << s.error_message();
    }
  }

  void StartCamera() {
    Status s;
    if (synched_) {
      s = camera_->Start(
          CameraFormat(640, 480, PIXEL_FORMAT_YUY2, 5),
          CameraFormat(640, 480, PIXEL_FORMAT_Z16, 5),
          ::base::BindRepeating(&DepthCameraPublishingNode::OnDepthCameraFrame,
                                ::base::Unretained(this)),
          ::base::BindRepeating(&DepthCameraPublishingNode::OnCameraError,
                                ::base::Unretained(this)));
    } else {
      s = camera_->Start(
          CameraFormat(640, 480, PIXEL_FORMAT_YUY2, 5),
          CameraFormat(640, 480, PIXEL_FORMAT_Z16, 5),
          ::base::BindRepeating(&DepthCameraPublishingNode::OnColorFrame,
                                ::base::Unretained(this)),
          ::base::BindRepeating(&DepthCameraPublishingNode::OnDepthFrame,
                                ::base::Unretained(this)),
          ::base::BindRepeating(&DepthCameraPublishingNode::OnCameraError,
                                ::base::Unretained(this)));
    }
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

  void OnDepthCameraFrame(CameraFrame color_frame, CameraFrame depth_frame) {
    std::cout << "DepthCameraPublishingNode::OnDepthCameraFrame" << std::endl;
    if (!color_publisher_.IsUnregistered()) {
      color_publisher_.Publish(
          color_frame.ToCameraFrameMessage(),
          ::base::BindOnce(&DepthCameraPublishingNode::OnPublishColor,
                           ::base::Unretained(this)));
    }
    if (!depth_publisher_.IsUnregistered()) {
      depth_publisher_.Publish(
          depth_frame.ToCameraFrameMessage(),
          ::base::BindOnce(&DepthCameraPublishingNode::OnPublishDepth,
                           ::base::Unretained(this)));
    }
  }

  void OnColorFrame(CameraFrame camera_frame) {
    std::cout << "DepthCameraPublishingNode::OnColorFrame" << std::endl;
    if (color_publisher_.IsUnregistered()) return;

    color_publisher_.Publish(
        camera_frame.ToCameraFrameMessage(),
        ::base::BindOnce(&DepthCameraPublishingNode::OnPublishColor,
                         ::base::Unretained(this)));
  }

  void OnDepthFrame(CameraFrame camera_frame) {
    std::cout << "DepthCameraPublishingNode::OnDepthFrame" << std::endl;
    if (depth_publisher_.IsUnregistered()) return;

    depth_publisher_.Publish(
        camera_frame.ToCameraFrameMessage(),
        ::base::BindOnce(&DepthCameraPublishingNode::OnPublishDepth,
                         ::base::Unretained(this)));
  }

  void OnCameraError(const Status& s) {
    std::cout << "DepthCameraPublishingNode::OnCameraError" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void OnPublishColor(const Status& s) {
    std::cout << "DepthCameraPublishingNode::OnPublishColor()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void OnPublishDepth(const Status& s) {
    std::cout << "DepthCameraPublishingNode::OnPublishDepth()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void RequestUnpublish() {
    color_publisher_.RequestUnpublish(
        node_info_, color_topic_,
        ::base::BindOnce(&DepthCameraPublishingNode::OnRequestUnpublish,
                         ::base::Unretained(this)));

    depth_publisher_.RequestUnpublish(
        node_info_, depth_topic_,
        ::base::BindOnce(&DepthCameraPublishingNode::OnRequestUnpublish,
                         ::base::Unretained(this)));
  }

  void OnRequestUnpublish(const Status& s) {
    std::cout << "DepthCameraPublishingNode::OnRequestUnpublish()" << std::endl;
    if (s.ok()) {
      if (!(color_publisher_.IsUnregistered() &&
            depth_publisher_.IsUnregistered()))
        return;

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
  std::string color_topic_;
  std::string depth_topic_;
  CameraDescriptor camera_descriptor_;
  bool synched_;
  Publisher<CameraFrameMessage> color_publisher_;
  Publisher<CameraFrameMessage> depth_publisher_;
  std::unique_ptr<DepthCameraInterface> camera_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_DEPTH_CAMERA_PUBLISHING_NODE_H_