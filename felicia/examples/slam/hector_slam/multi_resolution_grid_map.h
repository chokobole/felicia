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

#ifndef FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_MULTI_RESOLUTION_GRID_MAP_H_
#define FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_MULTI_RESOLUTION_GRID_MAP_H_

#include "felicia/core/lib/unit/geometry/size.h"
#include "felicia/examples/slam/hector_slam/log_odds_occupancy_grid_map.h"
#include "felicia/examples/slam/hector_slam/point_container.h"
#include "felicia/examples/slam/hector_slam/scan_matcher.h"

namespace felicia {
namespace hector_slam {

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

  map::OccupancyGridMapMessage ToOccupancyGridMapMessage(
      base::TimeDelta timestamp) const;

  void ToCsvFile(const base::FilePath& file_path) const;

 private:
  std::vector<std::unique_ptr<LogOddsOccupancyGridMap>> maps_;
  std::vector<PointContainer> point_containers_;
  ScanMatcher<LogOddsOccupancyGridMap> scan_matcher_;
};

}  // namespace hector_slam
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_MULTI_RESOLUTION_GRID_MAP_H_