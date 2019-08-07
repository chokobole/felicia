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

#include "felicia/examples/slam/hecto_slam/hector_slam.h"

namespace felicia {
namespace hector_slam {

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

}  // namespace hector_slam
}  // namespace felicia