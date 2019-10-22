#include "felicia/examples/slam/hector_slam/hector_slam_node.h"

namespace felicia {
namespace hector_slam {

HectorSlamNode::HectorSlamNode(const SlamNodeCreateFlag& slam_node_create_flag)
    : slam_node_create_flag_(slam_node_create_flag),
      lidar_topic_(slam_node_create_flag_.lidar_topic_flag()->value()),
      map_topic_(slam_node_create_flag_.map_topic_flag()->value()),
      pose_topic_(slam_node_create_flag_.pose_topic_flag()->value()),
      fps_(slam_node_create_flag_.lidar_fps_flag()->value()) {}

void HectorSlamNode::OnInit() {
  const HectorSlamFlag& hector_slam_flag =
      slam_node_create_flag_.hector_slam_delegate();
  int map_size = hector_slam_flag.map_size_flag()->value();
  float map_resolution = hector_slam_flag.map_resolution_flag()->value();
  float map_start_x = hector_slam_flag.map_start_x_flag()->value();
  float map_start_y = hector_slam_flag.map_start_y_flag()->value();
  int levels = hector_slam_flag.map_multi_res_levels_flag()->value();
  float map_update_distance_thresh =
      hector_slam_flag.map_update_distance_thresh_flag()->value();
  float map_update_angle_thresh =
      hector_slam_flag.map_update_angle_thresh_flag()->value();
  float update_factor_free =
      hector_slam_flag.update_factor_free_flag()->value();
  float update_factor_occupied =
      hector_slam_flag.update_factor_occupied_flag()->value();
  float laser_min_dist = hector_slam_flag.laser_min_dist_flag()->value();
  float laser_max_dist = hector_slam_flag.laser_max_dist_flag()->value();
  hector_slam_ = std::make_unique<HectorSlam>(
      this, Sizei{map_size, map_size}, map_resolution,
      Pointf{map_start_x, map_start_y}, levels, map_update_distance_thresh,
      map_update_angle_thresh, update_factor_free, update_factor_occupied,
      laser_min_dist, laser_max_dist);
}

void HectorSlamNode::OnDidCreate(NodeInfo node_info) {
  node_info_ = std::move(node_info);
  RequestSubscribe();
  RequestPublish();
}

void HectorSlamNode::OnPoseUpdated(const Posef& pose,
                                   base::TimeDelta timestamp) {
  if (pose_topic_.empty()) return;
  if (!pose_publisher_.IsRegistered()) return;
  pose_publisher_.Publish(PosefToPosefWithTimestampMessage(pose, timestamp));
}

void HectorSlamNode::OnMapUpdated(const MultiResolutionGridMap& map,
                                  base::TimeDelta timestamp) {
  if (map_topic_.empty()) return;
  if (!map_publisher_.IsRegistered()) return;
  map_publisher_.Publish(map.ToOccupancyGridMapMessage(timestamp));
}

void HectorSlamNode::OnMessage(drivers::LidarFrameMessage&& message) {
  drivers::LidarFrame lidar_frame;
  Status s = lidar_frame.FromLidarFrameMessage(std::move(message));
  if (s.ok()) hector_slam_->Update(std::move(lidar_frame));
}

void HectorSlamNode::RequestSubscribe() {
  communication::Settings settings;
  settings.queue_size = 1;
  settings.period = base::TimeDelta::FromSecondsD(1.0 / fps_);
  settings.is_dynamic_buffer = true;

  lidar_subscriber_.RequestSubscribe(
      node_info_, lidar_topic_,
      ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_SHM, settings,
      base::BindRepeating(&HectorSlamNode::OnMessage, base::Unretained(this)));
}

void HectorSlamNode::RequestPublish() {
  communication::Settings settings;
  settings.queue_size = 1;
  settings.is_dynamic_buffer = true;
  settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

  int channel_types =
      ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_WS;

  if (!pose_topic_.empty()) {
    pose_publisher_.RequestPublish(node_info_, pose_topic_, channel_types,
                                   settings);
  }

  if (!map_topic_.empty()) {
    map_publisher_.RequestPublish(node_info_, map_topic_, channel_types,
                                  settings);
  }
}

}  // namespace hector_slam
}  // namespace felicia