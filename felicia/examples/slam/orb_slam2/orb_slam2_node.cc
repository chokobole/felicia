#include "felicia/examples/slam/orb_slam2/orb_slam2_node.h"

#include "felicia/core/lib/coordinate/coordinate.h"
#include "felicia/core/lib/file/file_util.h"
#include "felicia/drivers/camera/camera_frame.h"
#include "felicia/drivers/camera/depth_camera_frame.h"
#include "felicia/map/pointcloud.h"

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
    rgbd_filter_.set_filter_callback(base::BindRepeating(
        &RGBDSynchronizer::Callback, base::Unretained(&rgbd_synchronizer)));
    rgbd_filter_.set_notify_callback(base::BindRepeating(
        &OrbSlam2Node::OnRGBDFrameMessage, base::Unretained(this)));
    rgbd_filter_.reserve(10);
  } else if (!left_color_topic_.empty() && !right_color_topic_.empty()) {
    sensor_type = ORB_SLAM2::System::STEREO;
    stereo_filter_.set_filter_callback(base::BindRepeating(
        &StereoSynchronizer::Callback, base::Unretained(&stereo_synchronizer)));
    stereo_filter_.set_notify_callback(base::BindRepeating(
        &OrbSlam2Node::OnStereoFrameMessage, base::Unretained(this)));
    stereo_filter_.reserve(10);
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

void OrbSlam2Node::OnDidCreate(NodeInfo node_info) {
  node_info_ = std::move(node_info);
  RequestSubscribe();
  RequestPublish();
}

void OrbSlam2Node::OnMonoFrameMessage(
    drivers::CameraFrameMessage&& mono_message) {
  drivers::CameraFrame mono_frame;
  Status s = mono_frame.FromCameraFrameMessage(std::move(mono_message));
  if (!s.ok()) return;
  cv::Mat mono_image;
  mono_frame.ToCvMat(&mono_image, false);
  cv::Mat w2c = orb_slam2_->TrackMonocular(mono_image,
                                           mono_frame.timestamp().InSecondsF());
  Publish(w2c, mono_frame.timestamp());
}

void OrbSlam2Node::OnRGBDFrameMessage(
    drivers::CameraFrameMessage&& rgb_message,
    drivers::DepthCameraFrameMessage&& depth_message) {
  drivers::CameraFrame rgb_frame;
  Status s = rgb_frame.FromCameraFrameMessage(std::move(rgb_message));
  if (!s.ok()) return;
  drivers::DepthCameraFrame depth_frame;
  s = depth_frame.FromDepthCameraFrameMessage(std::move(depth_message));
  if (!s.ok()) return;
  cv::Mat rgb_image;
  rgb_frame.ToCvMat(&rgb_image, false);
  cv::Mat depth_image;
  depth_frame.ToCvMat(&depth_image, false);
  cv::Mat w2c = orb_slam2_->TrackRGBD(rgb_image, depth_image,
                                      rgb_frame.timestamp().InSecondsF());
  Publish(w2c, rgb_frame.timestamp());
}

void OrbSlam2Node::OnStereoFrameMessage(
    drivers::CameraFrameMessage&& left_color_message,
    drivers::CameraFrameMessage&& right_color_message) {
  drivers::CameraFrame left_color_frame;
  Status s =
      left_color_frame.FromCameraFrameMessage(std::move(left_color_message));
  if (!s.ok()) return;
  drivers::CameraFrame right_color_frame;
  s = right_color_frame.FromCameraFrameMessage(std::move(right_color_message));
  if (!s.ok()) return;
  cv::Mat left_image;
  left_color_frame.ToCvMat(&left_image, false);
  cv::Mat right_image;
  right_color_frame.ToCvMat(&right_image, false);
  cv::Mat w2c = orb_slam2_->TrackStereo(
      left_image, right_image, left_color_frame.timestamp().InSecondsF());
  Publish(w2c, left_color_frame.timestamp());
}

void OrbSlam2Node::RequestSubscribe() {
  communication::Settings settings;
  settings.queue_size = 1;
  settings.period = base::TimeDelta::FromSecondsD(1.0 / color_fps_);
  settings.is_dynamic_buffer = true;

  int channel_types =
      ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_SHM;

  if (!left_color_topic_.empty()) {
    if (!depth_topic_.empty()) {
      left_color_subscriber_.RequestSubscribe(
          node_info_, left_color_topic_, channel_types, settings,
          base::BindRepeating(&RGBDMessageFilter::OnMessage<0>,
                              base::Unretained(&rgbd_filter_)));

      settings.period = base::TimeDelta::FromSecondsD(1.0 / depth_fps_);
      depth_subscriber_.RequestSubscribe(
          node_info_, depth_topic_, channel_types, settings,
          base::BindRepeating(&RGBDMessageFilter::OnMessage<1>,
                              base::Unretained(&rgbd_filter_)));
    } else if (!right_color_topic_.empty()) {
      left_color_subscriber_.RequestSubscribe(
          node_info_, left_color_topic_, channel_types, settings,
          base::BindRepeating(&StereoMessageFilter::OnMessage<0>,
                              base::Unretained(&stereo_filter_)));

      right_color_subscriber_.RequestSubscribe(
          node_info_, right_color_topic_, channel_types, settings,
          base::BindRepeating(&StereoMessageFilter::OnMessage<1>,
                              base::Unretained(&stereo_filter_)));
    } else {
      left_color_subscriber_.RequestSubscribe(
          node_info_, left_color_topic_, channel_types, settings,
          base::BindRepeating(&OrbSlam2Node::OnMonoFrameMessage,
                              base::Unretained(this)));
    }
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

void OrbSlam2Node::Publish(cv::Mat w2c, base::TimeDelta timestamp) {
  if (!frame_topic_.empty() && frame_publisher_.IsRegistered()) {
    cv::Mat frame = orb_slam2_->DrawCurrentFrame();
    drivers::CameraFormat camera_format(frame.cols, frame.rows,
                                        PIXEL_FORMAT_BGR, color_fps_);
    drivers::CameraFrameMessage message;
    message.set_data(frame.data, frame.total() * frame.elemSize());
    *message.mutable_camera_format() = camera_format.ToCameraFormatMessage();
    message.set_timestamp(timestamp.InMicroseconds());
    frame_publisher_.Publish(std::move(message));
  }

  if (!map_topic_.empty() && map_publisher_.IsRegistered()) {
    std::vector<ORB_SLAM2::MapPoint*> map_points =
        orb_slam2_->GetAllMapPoints();
    map::Pointcloud pointcloud;
    pointcloud.points().set_type(DATA_TYPE_32F_C3);
    pointcloud.points().reserve(map_points.size());
    Data::View<Point3f> points = pointcloud.points().AsView<Point3f>();
    for (ORB_SLAM2::MapPoint* map_point : map_points) {
      cv::Mat m = map_point->GetWorldPos();
      points.emplace_back(-m.at<float>(0), m.at<float>(2), -m.at<float>(1));
    }
    map_publisher_.Publish(pointcloud.ToPointcloudMessage(false));
  }

  if (!pose_topic_.empty() && pose_publisher_.IsRegistered()) {
    if (!w2c.empty()) {
      pose_publisher_.Publish(Pose3fWithTimestampFromCvMat(w2c, timestamp));
    }
  }
}

Pose3fWithTimestampMessage OrbSlam2Node::Pose3fWithTimestampFromCvMat(
    cv::Mat w2c, base::TimeDelta timestamp) const {
  Eigen::Matrix3f R;
  R << w2c.at<float>(0, 0), w2c.at<float>(0, 1), w2c.at<float>(0, 2),
      w2c.at<float>(1, 0), w2c.at<float>(1, 1), w2c.at<float>(1, 2),
      w2c.at<float>(2, 0), w2c.at<float>(2, 1), w2c.at<float>(2, 2);
  Eigen::Vector3f t;
  t << w2c.at<float>(0, 3), w2c.at<float>(1, 3), w2c.at<float>(2, 3);

  Eigen::Matrix3f orb_to_felicia;
  orb_to_felicia << -1, 0, 0, 0, 0, 1, 0, -1, 0;

  Eigen::Vector3f position = orb_to_felicia * R.transpose() * -t;
  Eigen::Quaternionf orientation(orb_to_felicia *
                                 (orb_to_felicia * R).transpose());

  Pose3f pose(Point3f{position}, Quaternionf{orientation});
  return Pose3fToPose3fWithTimestampMessage(pose, timestamp);
}

}  // namespace orb_slam2
}  // namespace felicia