#include "felicia/examples/slam/hecto_slam/hector_slam.h"

namespace felicia {

HectorSlam::HectorSlam(Client* client, Sizei map_size, float map_resolution,
                       const Pointf& start_coords, int levels,
                       float map_update_distance_thresh,
                       float map_update_angle_thresh, float update_factor_free,
                       float update_factor_occupied, float laser_min_dist,
                       float laser_max_dist)
    : client_(client),
      map_(map_size, map_resolution, start_coords, levels,
           update_factor_occupied, update_factor_free),
      last_map_updated_pose_(std::numeric_limits<float>::max(),
                             std::numeric_limits<float>::max(),
                             std::numeric_limits<float>::max()),
      map_update_distance_thresh_(map_update_distance_thresh),
      map_update_angle_thresh_(map_update_angle_thresh),
      laser_min_dist_(laser_min_dist),
      laser_max_dist_(laser_max_dist) {}

void HectorSlam::Update(LidarFrame&& lidar_frame) {
  std::vector<Pointf> points;
  lidar_frame.Project(&points, laser_min_dist_, laser_max_dist_);

  PointContainer point_container(std::move(points), map_.scale_to_map());

  Posef estimated = map_.Match(last_scan_matched_pose_, point_container);
  if (!estimated.IsValid()) {
    LOG(ERROR) << "Failed to match";
    return;
  }
  last_scan_matched_pose_ = estimated;
  client_->OnPoseUpdated(last_scan_matched_pose_, lidar_frame.timestamp());
  LOG(INFO) << "pose: " << last_scan_matched_pose_.point() << ", "
            << last_scan_matched_pose_.theta();

  float distance_diff =
      last_scan_matched_pose_.point().Distance(last_map_updated_pose_.point());
  float angle_diff = std::abs(last_scan_matched_pose_.theta() -
                              last_map_updated_pose_.theta());

  if (distance_diff > map_update_distance_thresh_ ||
      angle_diff > map_update_angle_thresh_) {
    map_.Update(last_scan_matched_pose_, point_container);
    client_->OnMapUpdated(map_, lidar_frame.timestamp());
    last_map_updated_pose_ = last_scan_matched_pose_;
  }
}

}  // namespace felicia