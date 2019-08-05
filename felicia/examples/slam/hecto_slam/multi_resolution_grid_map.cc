#include "felicia/examples/slam/hecto_slam/multi_resolution_grid_map.h"

namespace felicia {

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

OccupancyGridMapMessage MultiResolutionGridMap::ToOccupancyGridMapMessage(
    ::base::TimeDelta timestamp) const {
  return maps_[0]->ToOccupancyGridMapMessage(timestamp);
}

void MultiResolutionGridMap::ToCsvFile(
    const ::base::FilePath& file_path) const {
  maps_[0]->ToCsvFile(file_path);
}

}  // namespace felicia