#include "felicia/examples/learn/topic/camera/cc/camera_publishing_node.h"

#include "felicia/core/message/protobuf_util.h"
#include "felicia/drivers/camera/camera_factory.h"

namespace felicia {

CameraPublishingNode::CameraPublishingNode(
    const CameraFlag& camera_flag,
    const drivers::CameraDescriptor& camera_descriptor)
    : camera_flag_(camera_flag),
      topic_(camera_flag_.topic_flag()->value()),
      camera_descriptor_(camera_descriptor) {}

void CameraPublishingNode::OnInit() {
  camera_ = drivers::CameraFactory::NewCamera(camera_descriptor_);
  Status s = camera_->Init();
  CHECK(s.ok()) << s;

  // You can set camera settings here.
  drivers::CameraSettings camera_settings;
  s = camera_->SetCameraSettings(camera_settings);
  LOG_IF(ERROR, !s.ok()) << s;

  drivers::CameraSettingsInfoMessage message;
  s = camera_->GetCameraSettingsInfo(&message);
  if (s.ok()) {
    std::cout << protobuf::ProtobufMessageToString(message) << std::endl;
  } else {
    LOG(ERROR) << s;
  }
}

void CameraPublishingNode::OnDidCreate(NodeInfo node_info) {
  node_info_ = std::move(node_info);
  RequestPublish();
}

void CameraPublishingNode::OnRequestPublish(Status s) {
  if (s.ok()) {
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostTask(FROM_HERE,
                          base::BindOnce(&CameraPublishingNode::StartCamera,
                                         base::Unretained(this)));
  } else {
    LOG(ERROR) << s;
  }
}

void CameraPublishingNode::OnRequestUnpublish(Status s) {
  if (s.ok()) {
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostTask(FROM_HERE,
                          base::BindOnce(&CameraPublishingNode::StopCamera,
                                         base::Unretained(this)));
  } else {
    LOG(ERROR) << s;
  }
}

void CameraPublishingNode::RequestPublish() {
  communication::Settings settings;
  settings.queue_size = 1;
  settings.is_dynamic_buffer = true;
  settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

  publisher_.RequestPublish(
      node_info_, topic_,
      ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_SHM |
          ChannelDef::CHANNEL_TYPE_WS,
      settings,
      base::BindOnce(&CameraPublishingNode::OnRequestPublish,
                     base::Unretained(this)));
}

void CameraPublishingNode::RequestUnpublish() {
  publisher_.RequestUnpublish(
      node_info_, topic_,
      base::BindOnce(&CameraPublishingNode::OnRequestUnpublish,
                     base::Unretained(this)));
}

void CameraPublishingNode::StartCamera() {
  PixelFormat pixel_format;
  PixelFormat_Parse(camera_flag_.pixel_format_flag()->value(), &pixel_format);

  Status s = camera_->Start(
      drivers::CameraFormat(camera_flag_.width_flag()->value(),
                            camera_flag_.height_flag()->value(), pixel_format,
                            camera_flag_.fps_flag()->value()),
      base::BindRepeating(&CameraPublishingNode::OnCameraFrame,
                          base::Unretained(this)),
      base::BindRepeating(&CameraPublishingNode::OnCameraError,
                          base::Unretained(this)));
  if (s.ok()) {
    std::cout << "Camera Fomrat: " << camera_->camera_format() << std::endl;
    // MasterProxy& master_proxy = MasterProxy::GetInstance();
    // master_proxy.PostDelayedTask(
    //     FROM_HERE,
    //     base::BindOnce(&CameraPublishingNode::RequestUnpublish,
    //                      base::Unretained(this)),
    //     base::TimeDelta::FromSeconds(10));
  } else {
    LOG(ERROR) << s;
  }
}

void CameraPublishingNode::StopCamera() {
  Status s = camera_->Stop();
  LOG_IF(ERROR, !s.ok()) << s;
}

void CameraPublishingNode::OnCameraFrame(drivers::CameraFrame&& camera_frame) {
  if (publisher_.IsUnregistered()) return;

  publisher_.Publish(camera_frame.ToCameraFrameMessage(false));
}

void CameraPublishingNode::OnCameraError(Status s) { LOG(ERROR) << s; }

}  // namespace felicia