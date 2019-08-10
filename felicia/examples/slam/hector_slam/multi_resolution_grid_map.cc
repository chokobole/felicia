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

#include "felicia/examples/slam/hector_slam/multi_resolution_grid_map.h"

namespace felicia {
namespace hector_slam {

MultiResolutionGridMap::MultiResolutionGridMap(
    Sizei map_size, float map_resolution, const Pointf& start_coords,
    int levels, float occupied_factor, float free_factor) {
  Pointf origin(start_coords.x() * static_cast<float>(map_size.width()),
                start_coords.y() * static_cast<float>(map_size.height()));
  for (int i = 0; i < levels; ++i) {
    std::unique_ptr<LogOddsCellContainer> cell_container(
        new LogOddsCellContainer(map_size));
    std::unique_ptr<LogOddsOccupancyGridMap> map(
        new LogOddsOccupancyGridMap(std::move(cell_container), map_resolution,
                                    origin, occupied_factor, free_factor));
    maps_.push_back(std::move(map));

    map_size /= 2;
    origin /= 2;
    map_resolution *= 2.0f;
  }

  point_containers_.resize(levels - 1);
}

Posef MultiResolutionGridMap::Match(const Posef& pose,
                                    const PointContainer& point_container) {
  Posef pose_matched(pose);
  for (size_t i = maps_.size() - 1; i != SIZE_MAX; --i) {
    if (i == 0) {
      pose_matched = scan_matcher_.Match(pose_matched, point_container.points(),
                                         maps_[i].get(), 5);
    } else {
      point_containers_[i - 1].SetFrom(
          point_container,
          static_cast<float>(1.0 / pow(2.0, static_cast<double>(i))));
      pose_matched = scan_matcher_.Match(
          pose_matched, point_containers_[i - 1].points(), maps_[i].get(), 3);
    }
  }
  return pose_matched;
}

void MultiResolutionGridMap::Update(const Posef& pose,
                                    const PointContainer& point_container) {
  std::vector<float> unused;
  for (size_t i = 0; i < maps_.size(); ++i) {
    if (i == 0) {
      maps_[i]->Update(pose, point_container.origo(), point_container.points(),
                       unused);
    } else {
      maps_[i]->Update(pose, point_containers_[i - 1].origo(),
                       point_containers_[i - 1].points(), unused);
    }
  }
}

void MultiResolutionGridMap::SetOccupiedFactor(float prob) {
  for (auto& map : maps_) {
    map->SetOccupiedFactor(prob);
  }
}

void MultiResolutionGridMap::SetFreeFactor(float prob) {
  for (auto& map : maps_) {
    map->SetFreeFactor(prob);
  }
}

float MultiResolutionGridMap::scale_to_map() const {
  return maps_[0]->scale_to_map();
}

slam::OccupancyGridMapMessage MultiResolutionGridMap::ToOccupancyGridMapMessage(
    base::TimeDelta timestamp) const {
  return maps_[0]->ToOccupancyGridMapMessage(timestamp);
}

void MultiResolutionGridMap::ToCsvFile(const base::FilePath& file_path) const {
  maps_[0]->ToCsvFile(file_path);
}

}  // namespace hector_slam
}  // namespace felicia