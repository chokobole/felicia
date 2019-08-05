#ifndef FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_MULTI_RESOLUTION_GRID_MAP_H_
#define FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_MULTI_RESOLUTION_GRID_MAP_H_

#include "felicia/core/lib/unit/geometry/size.h"
#include "felicia/examples/slam/hecto_slam/log_odds_occupancy_grid_map.h"
#include "felicia/examples/slam/hecto_slam/point_container.h"
#include "felicia/examples/slam/hecto_slam/scan_matcher.h"

namespace felicia {

class MultiResolutionGridMap {
 public:
  MultiResolutionGridMap(Sizei map_size, float map_resolution,
                         const Pointf& start_coords, int levels,
                         float occupied_factor, float free_factor);

  Posef Match(const Posef& pose, const PointContainer& point_container);
  void Update(const Posef& pose, const PointContainer& point_container);

  void SetOccupiedFactor(float prob);
  void SetFreeFactor(float prob);

  float scale_to_map() const;

  OccupancyGridMapMessage ToOccupancyGridMapMessage(
      ::base::TimeDelta timestamp) const;

  void ToCsvFile(const ::base::FilePath& file_path) const;

 private:
  std::vector<std::unique_ptr<LogOddsOccupancyGridMap>> maps_;
  std::vector<PointContainer> point_containers_;
  ScanMatcher<LogOddsOccupancyGridMap> scan_matcher_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_MULTI_RESOLUTION_GRID_MAP_H_