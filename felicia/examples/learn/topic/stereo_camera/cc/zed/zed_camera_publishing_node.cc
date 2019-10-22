#include "felicia/examples/learn/topic/stereo_camera/cc/zed/zed_camera_publishing_node.h"

#include "felicia/core/message/protobuf_util.h"
#include "felicia/drivers/vendors/zed/zed_camera_factory.h"

namespace felicia {

ZedCameraPublishingNode::ZedCameraPublishingNode(
    const StereoCameraFlag& stereo_camera_flag,
    const drivers::ZedCameraDescriptor& camera_descriptor)
    : stereo_camera_flag_(stereo_camera_flag),
      left_camera_topic_(stereo_camera_flag_.left_camera_topic_flag()->value()),
      right_camera_topic_(
          stereo_camera_flag_.right_camera_topic_flag()->value()),
      depth_topic_(stereo_camera_flag_.depth_topic_flag()->value()),
      pointcloud_topic_(stereo_camera_flag_.pointcloud_topic_flag()->value()),
      camera_descriptor_(camera_descriptor) {}

void ZedCameraPublishingNode::OnInit() {
  camera_ = drivers::ZedCameraFactory::NewStereoCamera(camera_descriptor_);
  Status s = camera_->Init();
  CHECK(s.ok()) << s;
}

void ZedCameraPublishingNode::OnDidCreate(NodeInfo node_info) {
  node_info_ = std::move(node_info);
  RequestPublish();
}

void ZedCameraPublishingNode::OnRequestPublish(Status s) {
  if (s.ok()) {
    if (!left_camera_topic_.empty() && !left_camera_publisher_.IsRegistered())
      return;
    if (!right_camera_topic_.empty() && !right_camera_publisher_.IsRegistered())
      return;
    if (!depth_topic_.empty() && !depth_publisher_.IsRegistered()) return;
    if (!pointcloud_topic_.empty() && !pointcloud_publisher_.IsRegistered())
      return;

    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostTask(FROM_HERE,
                          base::BindOnce(&ZedCameraPublishingNode::StartCamera,
                                         base::Unretained(this)));
  } else {
    LOG(ERROR) << s;
  }
}

void ZedCameraPublishingNode::OnRequestUnpublish(Status s) {
  if (s.ok()) {
    if (!left_camera_topic_.empty() && !left_camera_publisher_.IsUnregistered())
      return;
    if (!right_camera_topic_.empty() &&
        !right_camera_publisher_.IsUnregistered())
      return;
    if (!depth_topic_.empty() && !depth_publisher_.IsUnregistered()) return;
    if (!pointcloud_topic_.empty() && !pointcloud_publisher_.IsUnregistered())
      return;

    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostTask(FROM_HERE,
                          base::BindOnce(&ZedCameraPublishingNode::StopCamera,
                                         base::Unretained(this)));
  } else {
    LOG(ERROR) << s;
  }
}

void ZedCameraPublishingNode::RequestPublish() {
  communication::Settings settings;
  settings.queue_size = 1;
  settings.is_dynamic_buffer = true;
  settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

  int channel_types =
      ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_WS;

  if (!left_camera_topic_.empty()) {
    left_camera_publisher_.RequestPublish(
        node_info_, left_camera_topic_, channel_types, settings,
        base::BindOnce(&ZedCameraPublishingNode::OnRequestPublish,
                       base::Unretained(this)));
  }

  if (!right_camera_topic_.empty()) {
    right_camera_publisher_.RequestPublish(
        node_info_, right_camera_topic_, channel_types, settings,
        base::BindOnce(&ZedCameraPublishingNode::OnRequestPublish,
                       base::Unretained(this)));
  }

  if (!depth_topic_.empty()) {
    depth_publisher_.RequestPublish(
        node_info_, depth_topic_, channel_types, settings,
        base::BindOnce(&ZedCameraPublishingNode::OnRequestPublish,
                       base::Unretained(this)));
  }

  if (!pointcloud_topic_.empty()) {
    pointcloud_publisher_.RequestPublish(
        node_info_, pointcloud_topic_, channel_types, settings,
        base::BindOnce(&ZedCameraPublishingNode::OnRequestPublish,
                       base::Unretained(this)));
  }
}

void ZedCameraPublishingNode::RequestUnpublish() {
  if (!left_camera_topic_.empty()) {
    left_camera_publisher_.RequestUnpublish(
        node_info_, left_camera_topic_,
        base::BindOnce(&ZedCameraPublishingNode::OnRequestUnpublish,
                       base::Unretained(this)));
  }

  if (!right_camera_topic_.empty()) {
    right_camera_publisher_.RequestUnpublish(
        node_info_, right_camera_topic_,
        base::BindOnce(&ZedCameraPublishingNode::OnRequestUnpublish,
                       base::Unretained(this)));
  }

  if (!depth_topic_.empty()) {
    depth_publisher_.RequestUnpublish(
        node_info_, depth_topic_,
        base::BindOnce(&ZedCameraPublishingNode::OnRequestUnpublish,
                       base::Unretained(this)));
  }

  if (!pointcloud_topic_.empty()) {
    pointcloud_publisher_.RequestUnpublish(
        node_info_, pointcloud_topic_,
        base::BindOnce(&ZedCameraPublishingNode::OnRequestUnpublish,
                       base::Unretained(this)));
  }
}

void ZedCameraPublishingNode::StartCamera() {
  drivers::ZedCamera::StartParams params;
  PixelFormat pixel_format;
  PixelFormat_Parse(stereo_camera_flag_.pixel_format_flag()->value(),
                    &pixel_format);

  params.requested_camera_format = drivers::CameraFormat(
      stereo_camera_flag_.width_flag()->value(),
      stereo_camera_flag_.height_flag()->value(), pixel_format,
      stereo_camera_flag_.fps_flag()->value());
  params.status_callback = base::BindRepeating(
      &ZedCameraPublishingNode::OnCameraError, base::Unretained(this));
  if (!left_camera_topic_.empty()) {
    params.left_camera_frame_callback = base::BindRepeating(
        &ZedCameraPublishingNode::OnLeftCameraFrame, base::Unretained(this));
  }
  if (!right_camera_topic_.empty()) {
    params.right_camera_frame_callback = base::BindRepeating(
        &ZedCameraPublishingNode::OnRightCameraFrame, base::Unretained(this));
  }
  if (!depth_topic_.empty()) {
    params.init_params.coordinate_units = sl::UNIT_MILLIMETER;
    params.init_params.coordinate_system =
        sl::COORDINATE_SYSTEM_LEFT_HANDED_Y_UP;
    params.depth_camera_frame_callback = base::BindRepeating(
        &ZedCameraPublishingNode::OnDepthFrame, base::Unretained(this));
  }
  if (!pointcloud_topic_.empty()) {
    // overwrite, because pointcloud calcuation is more complicated.
    params.init_params.coordinate_units = sl::UNIT_METER;
    params.init_params.coordinate_system =
        sl::COORDINATE_SYSTEM_LEFT_HANDED_Y_UP;
    params.pointcloud_interval = base::TimeDelta::FromSeconds(1);
    params.pointcloud_callback = base::BindRepeating(
        &ZedCameraPublishingNode::OnPointcloud, base::Unretained(this));
  }

  Status s = camera_->Start(params);
  if (s.ok()) {
    if (!left_camera_topic_.empty() || !right_camera_topic_.empty()) {
      std::cout << "Camera Fomrat: " << camera_->camera_format() << std::endl;
    }
    // MasterProxy& master_proxy = MasterProxy::GetInstance();
    // master_proxy.PostDelayedTask(
    //     FROM_HERE,
    //     base::BindOnce(&ZedCameraPublishingNode::RequestUnpublish,
    //                      base::Unretained(this)),
    //     base::TimeDelta::FromSeconds(10));
  } else {
    LOG(ERROR) << s;
  }
}

void ZedCameraPublishingNode::StopCamera() {
  Status s = camera_->Stop();
  LOG_IF(ERROR, !s.ok()) << s;
}

void ZedCameraPublishingNode::OnLeftCameraFrame(
    drivers::CameraFrame&& camera_frame) {
  if (left_camera_publisher_.IsUnregistered()) return;

  left_camera_publisher_.Publish(camera_frame.ToCameraFrameMessage(false));
}

void ZedCameraPublishingNode::OnRightCameraFrame(
    drivers::CameraFrame&& camera_frame) {
  if (right_camera_publisher_.IsUnregistered()) return;

  right_camera_publisher_.Publish(camera_frame.ToCameraFrameMessage(false));
}

void ZedCameraPublishingNode::OnDepthFrame(
    drivers::DepthCameraFrame&& depth_camera_frame) {
  if (depth_publisher_.IsUnregistered()) return;

  depth_publisher_.Publish(depth_camera_frame.ToDepthCameraFrameMessage(false));
}

void ZedCameraPublishingNode::OnPointcloud(map::Pointcloud&& pointcloud) {
  if (pointcloud_publisher_.IsUnregistered()) return;

  pointcloud_publisher_.Publish(pointcloud.ToPointcloudMessage(false));
}

void ZedCameraPublishingNode::OnCameraError(Status s) { LOG(ERROR) << s; }

}  // namespace felicia