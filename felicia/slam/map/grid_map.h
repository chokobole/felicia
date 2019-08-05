#ifndef FELICIA_SLAM_MAP_GRID_MAP_H_
#define FELICIA_SLAM_MAP_GRID_MAP_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/lib/unit/geometry/pose.h"
#include "felicia/core/lib/unit/geometry/size.h"
#include "felicia/core/lib/unit/geometry/transform.h"

namespace felicia {

template <typename CellType>
class GridMap {
 public:
  class CellContainer {
   public:
    virtual ~CellContainer() = default;

    virtual Sizei Size() const = 0;

    virtual bool HasCell(int x, int y) const = 0;

    virtual const CellType& At(int x, int y) const = 0;
    virtual CellType& At(int x, int y) = 0;
  };

  explicit GridMap(std::unique_ptr<CellContainer> cell_container)
      : cell_container_(std::move(cell_container)) {}
  GridMap(GridMap&& other) noexcept
      : cell_container_(std::move(other.cell_container_)),
        world_to_map_(std::move(other.world_to_map_)),
        map_to_world_(std::move(other.map_to_world_)),
        origin_(other.origin_),
        resolution_(other.resolution_),
        scale_to_map_(other.scale_to_map_) {}
  GridMap& operator=(GridMap&& other) = default;
  ~GridMap() = default;

  Sizei size() const { return cell_container_->Size(); }

  int width() const { return cell_container_->Size().width(); }

  int height() const { return cell_container_->Size().height(); }

  size_t area() const { return cell_container_->Size().area(); }

  void set_world_to_map(const Transformf& world_to_map) {
    world_to_map_ = world_to_map;
    map_to_world_ = world_to_map_.inverse();
  }

  void set_map_to_world(const Transformf& map_to_world) {
    map_to_world_ = map_to_world;
    world_to_map_ = map_to_world_.inverse();
  }

  const Transformf& map_to_world() const { return map_to_world_; }

  const Transformf& world_to_map() const { return world_to_map_; }

  Pointf ToWorldCoordinate(const Pointf& point) const {
    return point.Transform(map_to_world_);
  }

  Pointf ToMapCoordinate(const Pointf& point) const {
    return point.Transform(world_to_map_);
  }

  void set_origin(const Pointf& origin) { origin_ = origin; }

  const Pointf& origin() const { return origin_; }

  void set_resolution(float resolution) {
    resolution_ = resolution;
    scale_to_map_ = 1.f / resolution_;
  }

  void set_scale_to_map(float scale_to_map) {
    scale_to_map_ = scale_to_map;
    resolution_ = 1.f / scale_to_map_;
  }

  float scale_to_map() const { return scale_to_map_; }

  float resolution() const { return resolution_; }

 protected:
  std::unique_ptr<CellContainer> cell_container_;

  Transformf world_to_map_;
  Transformf map_to_world_;

  Pointf origin_;
  float resolution_;
  float scale_to_map_;

  DISALLOW_COPY_AND_ASSIGN(GridMap<CellType>);
};

}  // namespace felicia

#endif  // FELICIA_SLAM_MAP_GRID_MAP_H_