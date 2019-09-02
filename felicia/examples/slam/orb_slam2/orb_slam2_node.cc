#include "felicia/examples/slam/orb_slam2/orb_slam2_node.h"

#include "felicia/core/lib/coordinate/coordinate.h"
#include "felicia/core/lib/file/file_util.h"
#include "felicia/drivers/camera/camera_frame.h"
#include "felicia/drivers/pointcloud/pointcloud_frame.h"

namespace felicia {
namespace orb_slam2 {

OrbSlam2Node::OrbSlam2Node(const SlamNodeCreateFlag& slam_node_create_flag)
    : slam_node_create_flag_(slam_node_create_flag),
      left_color_topic_(
          slam_node_create_flag_.left_color_topic_flag()->value()),
      right_color_topic_(
          slam_node_create_flag_.right_color_topic_flag()->value()),
      depth_topic_(slam_node_create_flag_.depth_topic_flag()->value()),
      frame_topic_(slam_node_create_flag_.frame_topic_flag()->value()),
      map_topic_(slam_node_create_flag_.map_topic_flag()->value()),
      pose_topic_(slam_node_create_flag_.pose_topic_flag()->value()),
      color_fps_(slam_node_create_flag_.color_fps_flag()->value()),
      depth_fps_(slam_node_create_flag_.depth_fps_flag()->value()) {}

void OrbSlam2Node::OnInit() {
  ORB_SLAM2::System::eSensor sensor_type;
  if (!left_color_topic_.empty() && !depth_topic_.empty()) {
    sensor_type = ORB_SLAM2::System::RGBD;
  } else if (!left_color_topic_.empty() && !right_color_topic_.empty()) {
    sensor_type = ORB_SLAM2::System::STEREO;
  } else if (!left_color_topic_.empty()) {
    sensor_type = ORB_SLAM2::System::MONOCULAR;
  } else {
    NOTREACHED() << "Can't set sensor_type";
  }

  const OrbSlam2Flag& orb_slam2_flag =
      slam_node_create_flag_.orb_slam2_delegate();
  const std::string& voc_path = orb_slam2_flag.voc_path_flag()->value();
  const std::string& settings_path =
      orb_slam2_flag.settings_path_flag()->value();

  orb_slam2_ = std::make_unique<ORB_SLAM2::System>(voc_path, settings_path,
                                                   sensor_type, false);
}

void OrbSlam2Node::OnDidCreate(const NodeInfo& node_info) {
  node_info_ = node_info;
  RequestSubscribe();
  RequestPublish();
}

void OrbSlam2Node::OnLeftColorMessage(drivers::CameraFrameMessage&& message) {
#if defined(HAS_OPENCV)
  drivers::CameraFrame camera_frame;
  Status s = camera_frame.FromCameraFrameMessage(std::move(message));
  if (!s.ok()) return;
  base::TimeDelta timestamp =
      base::TimeDelta::FromMicroseconds(message.timestamp());
  cv::Mat left_image, right_image;
  if (camera_frame.ToCvMat(&left_image)) {
    Track(left_image, right_image, timestamp.InMicrosecondsF());
  }
#endif
}

void OrbSlam2Node::RequestSubscribe() {
  communication::Settings settings;
  settings.queue_size = 1;
  settings.period = base::TimeDelta::FromSecondsD(1.0 / color_fps_);
  settings.is_dynamic_buffer = true;

  if (!left_color_topic_.empty()) {
    left_color_subscriber_.RequestSubscribe(
        node_info_, left_color_topic_,
        ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_SHM, settings,
        base::BindRepeating(&OrbSlam2Node::OnLeftColorMessage,
                            base::Unretained(this)));
  }
}

void OrbSlam2Node::RequestPublish() {
  communication::Settings settings;
  settings.queue_size = 1;
  settings.is_dynamic_buffer = true;
  settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

  int channel_types =
      ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_WS;

  if (!frame_topic_.empty()) {
    frame_publisher_.RequestPublish(node_info_, frame_topic_, channel_types,
                                    settings);
  }

  if (!map_topic_.empty()) {
    map_publisher_.RequestPublish(node_info_, map_topic_, channel_types,
                                  settings);
  }

  if (!pose_topic_.empty()) {
    pose_publisher_.RequestPublish(node_info_, pose_topic_, channel_types,
                                   settings);
  }
}

void OrbSlam2Node::OnPoseUpdated(const Posef& pose, base::TimeDelta timestamp) {
  if (pose_topic_.empty()) return;
  if (!pose_publisher_.IsRegistered()) return;
  pose_publisher_.Publish(PosefToPosefWithTimestampMessage(pose, timestamp));
}

void OrbSlam2Node::Track(cv::Mat left_image, cv::Mat right_image,
                         double timestamp) {
  if (right_image.empty()) {
    orb_slam2_->TrackMonocular(left_image, timestamp);
  } else {
    orb_slam2_->TrackStereo(left_image, right_image, timestamp);
  }

  if (!frame_topic_.empty() && frame_publisher_.IsRegistered()) {
    cv::Mat frame = orb_slam2_->DrawCurrentFrame();
    drivers::CameraFormat camera_format(frame.cols, frame.rows,
                                        PIXEL_FORMAT_BGR, color_fps_);
    drivers::CameraFrameMessage message;
    message.set_data(frame.data, frame.total() * frame.elemSize());
    *message.mutable_camera_format() = camera_format.ToCameraFormatMessage();
    message.set_timestamp(timestamp);
    frame_publisher_.Publish(std::move(message));
  }

  if (!map_topic_.empty() && map_publisher_.IsRegistered()) {
    std::vector<ORB_SLAM2::MapPoint*> map_points =
        orb_slam2_->GetAllMapPoints();
    drivers::PointcloudFrame pointcloud;
    Coordinate from_coordinate(Coordinate::COORDINATE_SYSTEM_IMAGE);
    pointcloud.points().set_type(DATA_TYPE_32F_C3);
    Data::View<Point3f> points = pointcloud.points().AsView<Point3f>();
    for (ORB_SLAM2::MapPoint* map_point : map_points) {
      cv::Mat m = map_point->GetWorldPos();
      Point3f p(m.at<float>(0), m.at<float>(1), m.at<float>(2));
      points.push_back(
          from_coordinate.Convert(p, Coordinate::COORDINATE_SYSTEM_IMAGE));
    }
    map_publisher_.Publish(pointcloud.ToPointcloudFrameMessage(false));
  }
}

}  // namespace orb_slam2
}  // namespace felicia