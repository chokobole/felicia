#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_STEREO_CAMERA_CC_ZED_ZED_CAMERA_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_STEREO_CAMERA_CC_ZED_ZED_CAMERA_PUBLISHING_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/message/protobuf_util.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/vendors/zed/zed_camera_factory.h"

namespace felicia {

class ZedCameraPublishingNode : public NodeLifecycle {
 public:
  ZedCameraPublishingNode(const std::string& left_camera_topic,
                          const std::string& right_camera_topic,
                          const std::string& depth_camera_topic,
                          const CameraDescriptor& camera_descriptor)
      : left_camera_topic_(left_camera_topic),
        right_camera_topic_(right_camera_topic),
        depth_camera_topic_(depth_camera_topic),
        camera_descriptor_(camera_descriptor) {}

  void OnInit() override {
    std::cout << "ZedCameraPublishingNode::OnInit()" << std::endl;
    camera_ = ZedCameraFactory::NewStereoCamera(camera_descriptor_);
    Status s = camera_->Init();
    CHECK(s.ok()) << s;
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "ZedCameraPublishingNode::OnDidCreate()" << std::endl;
    node_info_ = node_info;
    RequestPublish();
  }

  void OnError(const Status& s) override {
    std::cout << "ZedCameraPublishingNode::OnError()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s;
  }

  void RequestPublish() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.is_dynamic_buffer = true;
    settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

    left_camera_publisher_.RequestPublish(
        node_info_, left_camera_topic_, ChannelDef::TCP | ChannelDef::WS,
        settings,
        ::base::BindOnce(&ZedCameraPublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));

    right_camera_publisher_.RequestPublish(
        node_info_, right_camera_topic_, ChannelDef::TCP | ChannelDef::WS,
        settings,
        ::base::BindOnce(&ZedCameraPublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));

    depth_camera_publisher_.RequestPublish(
        node_info_, depth_camera_topic_, ChannelDef::TCP | ChannelDef::WS,
        settings,
        ::base::BindOnce(&ZedCameraPublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));
  }

  void OnRequestPublish(const Status& s) {
    std::cout << "ZedCameraPublishingNode::OnRequestPublish()" << std::endl;
    if (s.ok()) {
      if (!(left_camera_publisher_.IsRegistered() &&
            right_camera_publisher_.IsRegistered() &&
            depth_camera_publisher_.IsRegistered()))
        return;

      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(
          FROM_HERE, ::base::BindOnce(&ZedCameraPublishingNode::StartCamera,
                                      ::base::Unretained(this)));
    } else {
      LOG(ERROR) << s;
    }
  }

  void StartCamera() {
    Status s = camera_->Start(
        CameraFormat(1280, 760, PIXEL_FORMAT_ARGB, 30),
        ::base::BindRepeating(&ZedCameraPublishingNode::OnLeftCameraFrame,
                              ::base::Unretained(this)),
        ::base::BindRepeating(&ZedCameraPublishingNode::OnRightCameraFrame,
                              ::base::Unretained(this)),
        ::base::BindRepeating(&ZedCameraPublishingNode::OnDepthCameraFrame,
                              ::base::Unretained(this)),
        ::base::BindRepeating(&ZedCameraPublishingNode::OnCameraError,
                              ::base::Unretained(this)));

    if (s.ok()) {
      // MasterProxy& master_proxy = MasterProxy::GetInstance();
      // master_proxy.PostDelayedTask(
      //     FROM_HERE,
      //     ::base::BindOnce(&ZedCameraPublishingNode::RequestUnpublish,
      //                      ::base::Unretained(this)),
      //     ::base::TimeDelta::FromSeconds(10));
    } else {
      LOG(ERROR) << s;
    }
  }

  void OnLeftCameraFrame(CameraFrame camera_frame) {
    if (left_camera_publisher_.IsUnregistered()) return;

    left_camera_publisher_.Publish(
        camera_frame.ToCameraFrameMessage(),
        ::base::BindRepeating(&ZedCameraPublishingNode::OnPublishLeftCamera,
                              ::base::Unretained(this)));
  }

  void OnRightCameraFrame(CameraFrame camera_frame) {
    if (right_camera_publisher_.IsUnregistered()) return;

    right_camera_publisher_.Publish(
        camera_frame.ToCameraFrameMessage(),
        ::base::BindRepeating(&ZedCameraPublishingNode::OnPublishRight,
                              ::base::Unretained(this)));
  }

  void OnDepthCameraFrame(DepthCameraFrame camera_frame) {
    if (depth_camera_publisher_.IsUnregistered()) return;

    depth_camera_publisher_.Publish(
        camera_frame.ToDepthCameraFrameMessage(),
        ::base::BindRepeating(&ZedCameraPublishingNode::OnPublishRight,
                              ::base::Unretained(this)));
  }

  void OnCameraError(const Status& s) { LOG_IF(ERROR, !s.ok()) << s; }

  void OnPublishLeftCamera(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

  void OnPublishRight(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

  void RequestUnpublish() {
    left_camera_publisher_.RequestUnpublish(
        node_info_, left_camera_topic_,
        ::base::BindOnce(&ZedCameraPublishingNode::OnRequestUnpublish,
                         ::base::Unretained(this)));

    right_camera_publisher_.RequestUnpublish(
        node_info_, right_camera_topic_,
        ::base::BindOnce(&ZedCameraPublishingNode::OnRequestUnpublish,
                         ::base::Unretained(this)));

    depth_camera_publisher_.RequestUnpublish(
        node_info_, right_camera_topic_,
        ::base::BindOnce(&ZedCameraPublishingNode::OnRequestUnpublish,
                         ::base::Unretained(this)));
  }

  void OnRequestUnpublish(const Status& s) {
    std::cout << "ZedCameraPublishingNode::OnRequestUnpublish()" << std::endl;
    if (s.ok()) {
      if (!(left_camera_publisher_.IsUnregistered() &&
            right_camera_publisher_.IsUnregistered() &&
            depth_camera_publisher_.IsUnregistered()))
        return;

      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(
          FROM_HERE, ::base::BindOnce(&ZedCameraPublishingNode::StopCamera,
                                      ::base::Unretained(this)));
    } else {
      LOG(ERROR) << s;
    }
  }

  void StopCamera() {
    Status s = camera_->Stop();
    LOG_IF(ERROR, !s.ok()) << s;
  }

 private:
  NodeInfo node_info_;
  std::string left_camera_topic_;
  std::string right_camera_topic_;
  std::string depth_camera_topic_;
  CameraDescriptor camera_descriptor_;
  Publisher<CameraFrameMessage> left_camera_publisher_;
  Publisher<CameraFrameMessage> right_camera_publisher_;
  Publisher<DepthCameraFrameMessage> depth_camera_publisher_;
  std::unique_ptr<ZedCamera> camera_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_STEREO_CAMERA_CC_ZED_ZED_CAMERA_PUBLISHING_NODE_H_