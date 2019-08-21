//=================================================================================================
// Copyright (c) 2011, Stefan Kohlbrecher, TU Darmstadt
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Simulation, Systems Optimization and Robotics
//       group, TU Darmstadt nor the names of its contributors may be used to
//       endorse or promote products derived from this software without
//       specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//=================================================================================================

#ifndef FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_NODE_H_
#define FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/communication/subscriber.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/lidar/lidar_frame.h"
#include "felicia/examples/slam/hector_slam/hector_slam.h"
#include "felicia/examples/slam/slam_node_create_flag.h"

namespace felicia {
namespace hector_slam {

class HectorSlamNode : public NodeLifecycle, public HectorSlam::Client {
 public:
  explicit HectorSlamNode(const SlamNodeCreateFlag& slam_node_create_flag)
      : slam_node_create_flag_(slam_node_create_flag),
        lidar_topic_(slam_node_create_flag_.lidar_topic_flag()->value()),
        map_topic_(slam_node_create_flag_.map_topic_flag()->value()),
        pose_topic_(slam_node_create_flag_.pose_topic_flag()->value()),
        fps_(slam_node_create_flag_.fps_flag()->value()) {}

  void OnInit() override {
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

  void OnDidCreate(const NodeInfo& node_info) override {
    node_info_ = node_info;
    RequestSubscribe();
    RequestPublish();
  }

  void OnError(const Status& s) override { LOG(ERROR) << s; }

  void RequestSubscribe() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.period = base::TimeDelta::FromSecondsD(1.0 / fps_);
    settings.is_dynamic_buffer = true;

    lidar_subscriber_.RequestSubscribe(
        node_info_, lidar_topic_,
        ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_SHM,
        base::BindRepeating(&HectorSlamNode::OnMessage, base::Unretained(this)),
        base::BindRepeating(&HectorSlamNode::OnSubscriptionError,
                            base::Unretained(this)),
        settings,
        base::BindOnce(&HectorSlamNode::OnRequestSubscribe,
                       base::Unretained(this)));
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
          base::BindOnce(&HectorSlamNode::OnRequestPublish,
                         base::Unretained(this)));
    }

    if (!map_topic_.empty()) {
      map_publisher_.RequestPublish(
          node_info_, map_topic_,
          ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_WS, settings,
          base::BindOnce(&HectorSlamNode::OnRequestPublish,
                         base::Unretained(this)));
    }
  }

  void OnMessage(drivers::LidarFrameMessage&& message) {
    drivers::LidarFrame lidar_frame;
    Status s = lidar_frame.FromLidarFrameMessage(std::move(message));
    if (s.ok()) hector_slam_->Update(std::move(lidar_frame));
  }

  void OnSubscriptionError(const Status& s) { LOG(ERROR) << s; }

  void OnRequestSubscribe(const Status& s) { LOG_IF(ERROR, !s.ok()) << s; }

  void OnRequestPublish(const Status& s) { LOG_IF(ERROR, !s.ok()) << s; }

  void OnPoseUpdated(const Posef& pose, base::TimeDelta timestamp) override {
    if (pose_topic_.empty()) return;
    if (!pose_publisher_.IsRegistered()) return;
    pose_publisher_.Publish(PosefToPosefWithTimestampMessage(pose, timestamp),
                            base::BindRepeating(&HectorSlamNode::OnPublishPose,
                                                base::Unretained(this)));
  }

  void OnMapUpdated(const MultiResolutionGridMap& map,
                    base::TimeDelta timestamp) override {
    if (map_topic_.empty()) return;
    if (!map_publisher_.IsRegistered()) return;
    slam::OccupancyGridMapMessage message =
        map.ToOccupancyGridMapMessage(timestamp);
    map_publisher_.Publish(std::move(message),
                           base::BindRepeating(&HectorSlamNode::OnPublishMap,
                                               base::Unretained(this)));
  }

  void OnPublishPose(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

  void OnPublishMap(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

 private:
  NodeInfo node_info_;
  const SlamNodeCreateFlag& slam_node_create_flag_;
  const std::string lidar_topic_;
  const std::string map_topic_;
  const std::string pose_topic_;
  const int fps_;
  std::unique_ptr<HectorSlam> hector_slam_;
  Subscriber<drivers::LidarFrameMessage> lidar_subscriber_;
  Publisher<PosefWithTimestampMessage> pose_publisher_;
  Publisher<slam::OccupancyGridMapMessage> map_publisher_;
};

}  // namespace hector_slam
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_NODE_H_