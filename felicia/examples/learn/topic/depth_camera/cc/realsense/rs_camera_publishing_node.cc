#include "felicia/examples/learn/topic/depth_camera/cc/realsense/rs_camera_publishing_node.h"

#include "felicia/drivers/vendors/realsense/rs_camera_factory.h"

namespace felicia {

RsCameraPublishingNode::RsCameraPublishingNode(
    const RsCameraFlag& rs_camera_flag,
    const drivers::CameraDescriptor& camera_descriptor)
    : rs_camera_flag_(rs_camera_flag),
      color_topic_(rs_camera_flag_.color_topic_flag()->value()),
      depth_topic_(rs_camera_flag_.depth_topic_flag()->value()),
      pointcloud_topic_(rs_camera_flag_.pointcloud_topic_flag()->value()),
      imu_topic_(rs_camera_flag_.imu_topic_flag()->value()),
      camera_descriptor_(camera_descriptor) {
  CHECK(!(color_topic_.empty() && depth_topic_.empty() && imu_topic_.empty() &&
          pointcloud_topic_.empty()))
      << "At least one of topic should be not empty";
}

void RsCameraPublishingNode::OnInit() {
  camera_ = drivers::RsCameraFactory::NewDepthCamera(camera_descriptor_);
  Status s = camera_->Init();
  CHECK(s.ok()) << s;
}

void RsCameraPublishingNode::OnDidCreate(NodeInfo node_info) {
  node_info_ = std::move(node_info);
  RequestPublish();
}

void RsCameraPublishingNode::OnRequestPublish(Status s) {
  if (s.ok()) {
    if (!color_topic_.empty() && !color_publisher_.IsRegistered()) return;
    if (!depth_topic_.empty() && !depth_publisher_.IsRegistered()) return;
    if (!pointcloud_topic_.empty() && !pointcloud_publisher_.IsRegistered())
      return;
    if (!imu_topic_.empty() && !imu_publisher_.IsRegistered()) return;

    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostTask(FROM_HERE,
                          base::BindOnce(&RsCameraPublishingNode::StartCamera,
                                         base::Unretained(this)));
  } else {
    LOG(ERROR) << s;
  }
}

void RsCameraPublishingNode::OnRequestUnpublish(Status s) {
  if (s.ok()) {
    if (!color_topic_.empty() && !color_publisher_.IsUnregistered()) return;
    if (!depth_topic_.empty() && !depth_publisher_.IsUnregistered()) return;
    if (!pointcloud_topic_.empty() && !pointcloud_publisher_.IsUnregistered())
      return;
    if (!imu_topic_.empty() && !imu_publisher_.IsUnregistered()) return;

    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostTask(FROM_HERE,
                          base::BindOnce(&RsCameraPublishingNode::StopCamera,
                                         base::Unretained(this)));
  } else {
    LOG(ERROR) << s;
  }
}

void RsCameraPublishingNode::RequestPublish() {
  communication::Settings settings;
  settings.queue_size = 1;
  settings.is_dynamic_buffer = true;
  settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

  int channel_types =
      ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_WS;

  if (!color_topic_.empty()) {
    color_publisher_.RequestPublish(
        node_info_, color_topic_, channel_types, settings,
        base::BindOnce(&RsCameraPublishingNode::OnRequestPublish,
                       base::Unretained(this)));
  }

  if (!depth_topic_.empty()) {
    depth_publisher_.RequestPublish(
        node_info_, depth_topic_, channel_types, settings,
        base::BindOnce(&RsCameraPublishingNode::OnRequestPublish,
                       base::Unretained(this)));
  }

  if (!pointcloud_topic_.empty()) {
    pointcloud_publisher_.RequestPublish(
        node_info_, pointcloud_topic_, channel_types, settings,
        base::BindOnce(&RsCameraPublishingNode::OnRequestPublish,
                       base::Unretained(this)));
  }

  if (!imu_topic_.empty()) {
    imu_publisher_.RequestPublish(
        node_info_, imu_topic_, channel_types, settings,
        base::BindOnce(&RsCameraPublishingNode::OnRequestPublish,
                       base::Unretained(this)));
  }
}

void RsCameraPublishingNode::RequestUnpublish() {
  if (!color_topic_.empty()) {
    color_publisher_.RequestUnpublish(
        node_info_, color_topic_,
        base::BindOnce(&RsCameraPublishingNode::OnRequestUnpublish,
                       base::Unretained(this)));
  }

  if (!depth_topic_.empty()) {
    depth_publisher_.RequestUnpublish(
        node_info_, depth_topic_,
        base::BindOnce(&RsCameraPublishingNode::OnRequestUnpublish,
                       base::Unretained(this)));
  }

  if (!pointcloud_topic_.empty()) {
    pointcloud_publisher_.RequestUnpublish(
        node_info_, pointcloud_topic_,
        base::BindOnce(&RsCameraPublishingNode::OnRequestUnpublish,
                       base::Unretained(this)));
  }

  if (!imu_topic_.empty()) {
    imu_publisher_.RequestUnpublish(
        node_info_, imu_topic_,
        base::BindOnce(&RsCameraPublishingNode::OnRequestUnpublish,
                       base::Unretained(this)));
  }
}

void RsCameraPublishingNode::StartCamera() {
  if (camera_->IsStarted()) return;

  StatusOr<rs2::sensor> status_or = camera_->sensor(drivers::RS_COLOR);
  if (status_or.ok()) {
    rs2::sensor& s = status_or.ValueOrDie();
    camera_->SetOption(s, RS2_OPTION_EMITTER_ENABLED, 1);
  }

  drivers::RsCamera::StartParams params;
  params.status_callback = base::BindRepeating(
      &RsCameraPublishingNode::OnCameraError, base::Unretained(this));

  if (!color_topic_.empty()) {
    PixelFormat pixel_format;
    PixelFormat_Parse(rs_camera_flag_.pixel_format_flag()->value(),
                      &pixel_format);
    params.requested_color_format = drivers::CameraFormat(
        rs_camera_flag_.width_flag()->value(),
        rs_camera_flag_.height_flag()->value(), pixel_format,
        rs_camera_flag_.fps_flag()->value());
    params.color_frame_callback = base::BindRepeating(
        &RsCameraPublishingNode::OnColorFrame, base::Unretained(this));
  }

  if (!depth_topic_.empty()) {
    params.requested_depth_format = drivers::CameraFormat(
        rs_camera_flag_.width_flag()->value(),
        rs_camera_flag_.height_flag()->value(), PIXEL_FORMAT_Z16,
        rs_camera_flag_.fps_flag()->value());
    params.depth_frame_callback = base::BindRepeating(
        &RsCameraPublishingNode::OnDepthFrame, base::Unretained(this));
  }

  if (!pointcloud_topic_.empty()) {
    StatusOr<rs2::sensor> status_or = camera_->sensor(drivers::RS_COLOR);
    if (status_or.ok()) {
      params.named_filters.push_back(drivers::RsCamera::NamedFilter(
          drivers::RsCamera::NamedFilter::POINTCLOUD,
          std::make_shared<rs2::pointcloud>(drivers::RS_COLOR.stream_type,
                                            drivers::RS_COLOR.stream_index)));
    } else {
      status_or = camera_->sensor(drivers::RS_DEPTH);
      if (status_or.ok()) {
        params.named_filters.push_back(drivers::RsCamera::NamedFilter(
            drivers::RsCamera::NamedFilter::COLORIZER,
            std::make_shared<rs2::colorizer>()));
        params.named_filters.push_back(drivers::RsCamera::NamedFilter(
            drivers::RsCamera::NamedFilter::POINTCLOUD,
            std::make_shared<rs2::pointcloud>(drivers::RS_DEPTH.stream_type,
                                              drivers::RS_DEPTH.stream_index)));
      }
    }
    params.pointcloud_interval = base::TimeDelta::FromSeconds(1);
    params.pointcloud_callback = base::BindRepeating(
        &RsCameraPublishingNode::OnPointcloud, base::Unretained(this));
  }

  if (!imu_topic_.empty()) {
    params.requested_accel_format = drivers::ImuFormat(63);
    params.requested_gyro_format = drivers::ImuFormat(200);
    params.gyro_interval = base::TimeDelta::FromMilliseconds(100);
    params.accel_interval = base::TimeDelta::FromMilliseconds(100);
    params.imu_filter_kind = drivers::ImuFilterFactory::MADGWICK_FILTER_KIND;
    params.imu_frame_callback = base::BindRepeating(
        &RsCameraPublishingNode::OnImuFrame, base::Unretained(this));
  }

  Status s = camera_->Start(params);
  if (s.ok()) {
    if (!color_topic_.empty()) {
      std::cout << "Color Fomrat: " << camera_->color_format() << std::endl;
    }
    if (!depth_topic_.empty()) {
      std::cout << "Depth Format: " << camera_->depth_format() << std::endl;
    }
    if (!imu_topic_.empty()) {
      std::cout << "Accel Fomrat: " << camera_->accel_format() << std::endl;
      std::cout << "Gyro Format: " << camera_->gyro_format() << std::endl;
    }
    // MasterProxy& master_proxy = MasterProxy::GetInstance();
    // master_proxy.PostDelayedTask(
    //     FROM_HERE,
    //     base::BindOnce(&RsCameraPublishingNode::RequestUnpublish,
    //                      base::Unretained(this)),
    //     base::TimeDelta::FromSeconds(10));
  } else {
    LOG(ERROR) << s;
  }
}

void RsCameraPublishingNode::StopCamera() {
  Status s = camera_->Stop();
  LOG_IF(ERROR, !s.ok()) << s;
}

void RsCameraPublishingNode::OnColorFrame(drivers::CameraFrame&& color_frame) {
  if (color_publisher_.IsUnregistered()) return;

  color_publisher_.Publish(color_frame.ToCameraFrameMessage(false));
}

void RsCameraPublishingNode::OnDepthFrame(
    drivers::DepthCameraFrame&& depth_frame) {
  if (depth_publisher_.IsUnregistered()) return;

  depth_publisher_.Publish(depth_frame.ToDepthCameraFrameMessage(false));
}

void RsCameraPublishingNode::OnPointcloud(map::Pointcloud&& pointcloud) {
  if (pointcloud_publisher_.IsUnregistered()) return;

  pointcloud_publisher_.Publish(pointcloud.ToPointcloudMessage(false));
}

void RsCameraPublishingNode::OnImuFrame(const drivers::ImuFrame& imu_frame) {
  if (imu_publisher_.IsUnregistered()) return;

  imu_publisher_.Publish(imu_frame.ToImuFrameMessage());
}

void RsCameraPublishingNode::OnCameraError(Status s) { LOG(ERROR) << s; }

}  // namespace felicia