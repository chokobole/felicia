#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_PUBLISHING_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/message/protobuf_util.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/camera/camera_factory.h"

namespace felicia {

class CameraPublishingNode : public NodeLifecycle {
 public:
  CameraPublishingNode(const std::string& topic,
                       const CameraDescriptor& camera_descriptor)
      : topic_(topic), camera_descriptor_(camera_descriptor) {}

  void OnInit() override {
    std::cout << "CameraPublishingNode::OnInit()" << std::endl;
    camera_ = CameraFactory::NewCamera(camera_descriptor_);
    Status s = camera_->Init();
    CHECK(s.ok()) << s;

    // You can set camera settings here.
    CameraSettings camera_settings;
    s = camera_->SetCameraSettings(camera_settings);
    LOG_IF(ERROR, !s.ok()) << s;

    CameraSettingsInfoMessage message;
    s = camera_->GetCameraSettingsInfo(&message);
    if (s.ok()) {
      std::cout << protobuf::ProtobufMessageToString(message) << std::endl;
    } else {
      LOG(ERROR) << s;
    }
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "CameraPublishingNode::OnDidCreate()" << std::endl;
    node_info_ = node_info;
    RequestPublish();
  }

  void OnError(const Status& s) override {
    std::cout << "CameraPublishingNode::OnError()" << std::endl;
    LOG(ERROR) << s;
  }

  void RequestPublish() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.is_dynamic_buffer = true;
    settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

    publisher_.RequestPublish(
        node_info_, topic_, ChannelDef::TCP | ChannelDef::WS, settings,
        ::base::BindOnce(&CameraPublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));
  }

  void OnRequestPublish(const Status& s) {
    std::cout << "CameraPublishingNode::OnRequestPublish()" << std::endl;
    if (s.ok()) {
      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(FROM_HERE,
                            ::base::BindOnce(&CameraPublishingNode::StartCamera,
                                             ::base::Unretained(this)));
    } else {
      LOG(ERROR) << s;
    }
  }

  void StartCamera() {
    // You should set the camera format if you have any you want to run with.
    Status s = camera_->Start(
        CameraFormat(640, 480, PIXEL_FORMAT_BGR, 25),
        ::base::BindRepeating(&CameraPublishingNode::OnCameraFrame,
                              ::base::Unretained(this)),
        ::base::BindRepeating(&CameraPublishingNode::OnCameraError,
                              ::base::Unretained(this)));
    if (s.ok()) {
      std::cout << "Camera Fomrat: " << camera_->camera_format() << std::endl;
      // MasterProxy& master_proxy = MasterProxy::GetInstance();
      // master_proxy.PostDelayedTask(
      //     FROM_HERE,
      //     ::base::BindOnce(&CameraPublishingNode::RequestUnpublish,
      //                      ::base::Unretained(this)),
      //     ::base::TimeDelta::FromSeconds(10));
    } else {
      LOG(ERROR) << s;
    }
  }

  void OnCameraFrame(CameraFrame camera_frame) {
    if (publisher_.IsUnregistered()) return;

    publisher_.Publish(camera_frame.ToCameraFrameMessage(),
                       ::base::BindRepeating(&CameraPublishingNode::OnPublish,
                                             ::base::Unretained(this)));
  }

  void OnCameraError(const Status& s) { LOG(ERROR) << s; }

  void OnPublish(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

  void RequestUnpublish() {
    publisher_.RequestUnpublish(
        node_info_, topic_,
        ::base::BindOnce(&CameraPublishingNode::OnRequestUnpublish,
                         ::base::Unretained(this)));
  }

  void OnRequestUnpublish(const Status& s) {
    std::cout << "CameraPublishingNode::OnRequestUnpublish()" << std::endl;
    if (s.ok()) {
      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(FROM_HERE,
                            ::base::BindOnce(&CameraPublishingNode::StopCamera,
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
  std::string topic_;
  CameraDescriptor camera_descriptor_;
  Publisher<CameraFrameMessage> publisher_;
  std::unique_ptr<CameraInterface> camera_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_PUBLISHING_NODE_H_