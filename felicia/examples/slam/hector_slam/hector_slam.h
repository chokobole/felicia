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

#ifndef FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_H_
#define FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_H_

#include "felicia/drivers/lidar/lidar_frame.h"
#include "felicia/examples/slam/hector_slam/multi_resolution_grid_map.h"

namespace felicia {
namespace hector_slam {

class HectorSlam {
 public:
  class Client {
   public:
    virtual ~Client() = default;

    virtual void OnPoseUpdated(const Posef& pose,
                               base::TimeDelta timestamp) = 0;
    virtual void OnMapUpdated(const MultiResolutionGridMap& map,
                              base::TimeDelta timestamp) = 0;
  };

  HectorSlam(Client* client, Sizei map_size, float map_resolution,
             const Pointf& start_coords, int levels,
             float map_update_distance_thresh, float map_update_angle_thresh,
             float update_factor_free, float update_factor_occupied,
             float laser_min_dist, float laser_max_dist);

  void Update(LidarFrame&& lidar_frame);

 private:
  Client* client_;
  MultiResolutionGridMap map_;
  Posef last_scan_matched_pose_;
  Posef last_map_updated_pose_;
  float map_update_distance_thresh_;
  float map_update_angle_thresh_;
  float laser_min_dist_;
  float laser_max_dist_;
};

}  // namespace hector_slam
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_H_