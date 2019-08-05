#ifndef FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_NODE_H_
#define FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/communication/subscriber.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/lidar/lidar_frame.h"
#include "felicia/examples/slam/hecto_slam/hector_slam.h"
#include "felicia/examples/slam/hecto_slam/hector_slam_flag.h"

namespace felicia {

class HectorSlamNode : public NodeLifecycle, public HectorSlam::Client {
 public:
  HectorSlamNode(const std::string& lidar_topic, const std::string& map_topic,
                 const std::string& pose_topic,
                 const HectorSlamFlag& hector_slam_flag)
      : lidar_topic_(lidar_topic),
        map_topic_(map_topic),
        pose_topic_(pose_topic),
        hector_slam_flag_(hector_slam_flag) {}

  void OnInit() override {
    int map_size = hector_slam_flag_.map_size_flag()->value();
    float map_resolution = hector_slam_flag_.map_resolution_flag()->value();
    float map_start_x = hector_slam_flag_.map_start_x_flag()->value();
    float map_start_y = hector_slam_flag_.map_start_y_flag()->value();
    int levels = hector_slam_flag_.map_multi_res_levels_flag()->value();
    float map_update_distance_thresh =
        hector_slam_flag_.map_update_distance_thresh_flag()->value();
    float map_update_angle_thresh =
        hector_slam_flag_.map_update_angle_thresh_flag()->value();
    float update_factor_free =
        hector_slam_flag_.update_factor_free_flag()->value();
    float update_factor_occupied =
        hector_slam_flag_.update_factor_occupied_flag()->value();
    float laser_min_dist = hector_slam_flag_.laser_min_dist_flag()->value();
    float laser_max_dist = hector_slam_flag_.laser_max_dist_flag()->value();
    hector_slam_ = std::make_unique<HectorSlam>(
        this, Sizei{map_size, map_size}, map_resolution,
        Pointf{map_start_x, map_start_y}, levels, map_update_distance_thresh,
        map_update_angle_thresh, update_factor_free, update_factor_occupied,
        laser_min_dist, laser_max_dist);
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    node_info_ = node_info;
    RequestSubscribe();
    RequestPublish();
  }

  void OnError(const Status& s) override { LOG(ERROR) << s; }

  void RequestSubscribe() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.period = ::base::TimeDelta::FromMilliseconds(100);
    settings.is_dynamic_buffer = true;

    lidar_subscriber_.RequestSubscribe(
        node_info_, lidar_topic_,
        ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_SHM,
        ::base::BindRepeating(&HectorSlamNode::OnMessage,
                              ::base::Unretained(this)),
        ::base::BindRepeating(&HectorSlamNode::OnSubscriptionError,
                              ::base::Unretained(this)),
        settings,
        ::base::BindOnce(&HectorSlamNode::OnRequestSubscribe,
                         ::base::Unretained(this)));
  }

  void RequestPublish() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.is_dynamic_buffer = true;
    settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

    if (!pose_topic_.empty()) {
      pose_publisher_.RequestPublish(
          node_info_, pose_topic_,
          ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_WS, settings,
          ::base::BindOnce(&HectorSlamNode::OnRequestPublish,
                           ::base::Unretained(this)));
    }

    if (!map_topic_.empty()) {
      map_publisher_.RequestPublish(
          node_info_, map_topic_,
          ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_WS, settings,
          ::base::BindOnce(&HectorSlamNode::OnRequestPublish,
                           ::base::Unretained(this)));
    }
  }

  void OnMessage(LidarFrameMessage&& message) {
    LidarFrame lidar_frame = LidarFrame::FromLidarFrameMessage(message);
    hector_slam_->Update(std::move(lidar_frame));
  }

  void OnSubscriptionError(const Status& s) { LOG(ERROR) << s; }

  void OnRequestSubscribe(const Status& s) { LOG_IF(ERROR, !s.ok()) << s; }

  void OnRequestPublish(const Status& s) { LOG_IF(ERROR, !s.ok()) << s; }

  void OnPoseUpdated(const Posef& pose, ::base::TimeDelta timestamp) override {
    if (pose_topic_.empty()) return;
    if (!pose_publisher_.IsRegistered()) return;
    pose_publisher_.Publish(
        PosefToPosefWithTimestampMessage(pose, timestamp),
        ::base::BindRepeating(&HectorSlamNode::OnPublishPose,
                              ::base::Unretained(this)));
  }

  void OnMapUpdated(const MultiResolutionGridMap& map,
                    ::base::TimeDelta timestamp) override {
    if (map_topic_.empty()) return;
    if (!map_publisher_.IsRegistered()) return;
    OccupancyGridMapMessage message = map.ToOccupancyGridMapMessage(timestamp);
    map_publisher_.Publish(std::move(message),
                           ::base::BindRepeating(&HectorSlamNode::OnPublishMap,
                                                 ::base::Unretained(this)));
  }

  void OnPublishPose(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

  void OnPublishMap(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

 private:
  NodeInfo node_info_;
  const std::string lidar_topic_;
  const std::string map_topic_;
  const std::string pose_topic_;
  const HectorSlamFlag& hector_slam_flag_;
  std::unique_ptr<HectorSlam> hector_slam_;
  Subscriber<LidarFrameMessage> lidar_subscriber_;
  Publisher<PosefWithTimestampMessage> pose_publisher_;
  Publisher<OccupancyGridMapMessage> map_publisher_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_NODE_H_