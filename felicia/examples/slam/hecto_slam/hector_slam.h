#ifndef FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_H_
#define FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_H_

#include "felicia/drivers/lidar/lidar_frame.h"
#include "felicia/examples/slam/hecto_slam/multi_resolution_grid_map.h"

namespace felicia {

class HectorSlam {
 public:
  class Client {
   public:
    virtual ~Client() = default;

    virtual void OnPoseUpdated(const Posef& pose,
                               ::base::TimeDelta timestamp) = 0;
    virtual void OnMapUpdated(const MultiResolutionGridMap& map,
                              ::base::TimeDelta timestamp) = 0;
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

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_H_