#ifndef FELICIA_SLAM_MAP_OCCUPANCY_GRID_MAP_H_
#define FELICIA_SLAM_MAP_OCCUPANCY_GRID_MAP_H_

#include <vector>

#include "third_party/chromium/base/files/file.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/math/math_util.h"
#include "felicia/core/lib/unit/geometry/pose.h"
#include "felicia/core/lib/unit/geometry/transform.h"
#include "felicia/slam/map/grid_map.h"
#include "felicia/slam/map/map_message.pb.h"

namespace felicia {

template <typename CellType>
class OccupancyGridMap : public GridMap<CellType> {
 public:
  explicit OccupancyGridMap(
      std::unique_ptr<typename GridMap<CellType>::CellContainer> cell_container)
      : GridMap<CellType>(std::move(cell_container)) {}
  OccupancyGridMap(OccupancyGridMap&& other) = default;
  OccupancyGridMap& operator=(OccupancyGridMap&& other) = default;

  virtual ~OccupancyGridMap() = default;

  virtual bool IsOccupied(int x, int y) const = 0;
  virtual bool IsFree(int x, int y) const = 0;

  virtual void MarkOccupied(int x, int y) = 0;
  virtual void MarkFree(int x, int y) = 0;

  virtual bool HasValue(int x, int y) const = 0;
  virtual float Value(int x, int y) const = 0;

  // Should return row-major ordered width * height data.
  virtual std::string ToMapData() const = 0;

  // if |with_derivatives| is set to true, then it returns array of size 3,
  // which is interpolated value and gradient of value. Otherwise it returns
  // interpolated value.
  void InterpolatedValue(float x, float y, float* values,
                         bool with_derivatives = false) const;

  // Given world robot state |pose|, start point |point| and |points|, which
  // lidar found, it updates the occupancy grid map by using
  // Bresenham's algorithm.
  void Update(const Posef& pose, const Pointf& point,
              const std::vector<Pointf>& points,
              const std::vector<float>& intencities);

  OccupancyGridMapMessage ToOccupancyGridMapMessage(
      base::TimeDelta timestamp) const;

  void ToCsvFile(const base::FilePath& file_path) const;

 private:
  void Update(const Pointf& point, const Pointf& point2, float intencity);

  DISALLOW_COPY_AND_ASSIGN(OccupancyGridMap<CellType>);
};

template <typename CellType>
void OccupancyGridMap<CellType>::InterpolatedValue(
    float x, float y, float* values, bool with_derivatives) const {
  int p00_x = static_cast<int>(x);
  int p00_y = static_cast<int>(y);

  if (!HasValue(p00_x, p00_y)) {
    values[0] = 0;
    if (with_derivatives) {
      values[1] = 0;
      values[2] = 0;
    }
    return;
  }

  int p01_x = p00_x;
  int p01_y = p00_y + 1;

  int p10_x = p00_x + 1;
  int p10_y = p00_y;

  int p11_x = p00_x + 1;
  int p11_y = p00_y + 1;

  float x_ratio = x - static_cast<float>(p00_x);
  float y_ratio = y - static_cast<float>(p00_y);

  float v00 = Value(p00_x, p00_y);
  float v01 = Value(p01_x, p01_y);
  float v10 = Value(p10_x, p10_y);
  float v11 = Value(p11_x, p11_y);

  values[0] = y_ratio * (x_ratio * v11 + (1.f - x_ratio) * v01) +
              (1.f - y_ratio) * (x_ratio * v10 + (1.f - x_ratio) * v00);
  if (with_derivatives) {
    values[1] = y_ratio * (v11 - v01) + (1.f - y_ratio) * (v10 - v00);
    values[2] = x_ratio * (v11 - v10) + (1.f - x_ratio) * (v01 - v00);
  }
}

template <typename CellType>
void OccupancyGridMap<CellType>::Update(const Posef& pose, const Pointf& point,
                                        const std::vector<Pointf>& points,
                                        const std::vector<float>& intencities) {
  Pointf map_point = this->ToMapCoordinate(pose.point());
  Transformf transform;
  transform.AddRotation(pose.theta())
      .AddTranslate(map_point.x(), map_point.y());
  Pointf start_point = point.Transform(transform);

  if (intencities.size() > 0) {
    DCHECK_EQ(intencities.size(), points.size());
    for (size_t i = 0; i < points.size(); ++i) {
      Update(start_point, points[i].Transform(transform), intencities[i]);
    }
  } else {
    for (const Pointf& point : points) {
      Update(start_point, point.Transform(transform), -1.f);
    }
  }
}

template <typename CellType>
void OccupancyGridMap<CellType>::Update(const Pointf& point,
                                        const Pointf& point2,
                                        float intencity /* not used yet */) {
  int x = static_cast<int>(point.x());
  int y = static_cast<int>(point.y());
  int x2 = static_cast<int>(point2.x());
  int y2 = static_cast<int>(point2.y());

  if (!(HasValue(x, y) && HasValue(x2, y2))) return;

  if (x == x2 && y == y2) return;

  int dx = static_cast<int>(point2.x() - point.x());
  int dy = static_cast<int>(point2.y() - point.y());
  int abs_dx = std::abs(dx);
  int abs_dy = std::abs(dy);
  int offset_dx = sign(dx);
  int offset_dy = sign(dy);

  MarkFree(x, y);
  if (abs_dy >= abs_dx) {
    int error_y = static_cast<int>(abs_dy / 2);
    for (; y != y2; y += offset_dy) {
      error_y += abs_dx;
      if (error_y >= abs_dy) {
        error_y -= abs_dy;
        x += offset_dx;
      }

      MarkFree(x, y);
    }
  } else {
    int error_x = static_cast<int>(abs_dx / 2);
    for (; x != x2; x += offset_dx) {
      error_x += abs_dy;
      if (error_x >= abs_dx) {
        error_x -= abs_dx;
        y += offset_dy;
      }

      MarkFree(x, y);
    }
  }
  MarkOccupied(x2, y2);
}

template <typename CellType>
OccupancyGridMapMessage OccupancyGridMap<CellType>::ToOccupancyGridMapMessage(
    base::TimeDelta timestamp) const {
  OccupancyGridMapMessage message;
  *message.mutable_size() = SizeiToSizeiMessage(this->size());
  message.set_resolution(this->resolution_);
  *message.mutable_origin() = PointfToPointfMessage(this->origin_);
  message.set_data(ToMapData());
  message.set_timestamp(timestamp.InMicroseconds());
  return message;
}

template <typename CellType>
void OccupancyGridMap<CellType>::ToCsvFile(
    const base::FilePath& file_path) const {
  base::File file(file_path,
                  base::File::FLAG_CREATE_ALWAYS | base::File::FLAG_WRITE);
  std::string data = ToMapData();
  std::string csv_data;
  csv_data.reserve(data.length() * 2);
  for (size_t i = 0; i < data.length(); ++i) {
    csv_data.push_back(data[i]);
    if ((i + 1) % this->width() == 0) {
      csv_data.push_back('\n');
    } else {
      csv_data.push_back(',');
    }
  }
  file.Write(0, csv_data.c_str(), csv_data.length());
}

}  // namespace felicia

#endif  // FELICIA_SLAM_MAP_OCCUPANCY_GRID_MAP_H_