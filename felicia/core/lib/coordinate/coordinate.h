#ifndef FELICIA_CORE_LIB_COORDINATE_COORDINATE_H_
#define FELICIA_CORE_LIB_COORDINATE_COORDINATE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/point.h"

namespace felicia {

class EXPORT Coordinate {
 public:
  // This was taken and modified from zed/include/sl_core/utils/types.hpp
  enum CoordinateSystem {
    // Standard coordinates system in computer vision.
    // Used in OpenCV : see here
    // http://docs.opencv.org/2.4/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html
    COORDINATE_SYSTEM_IMAGE,
    // Left-Handed with Y up and Z forward. Used in Unity with DirectX and
    // BabylonJs.
    COORDINATE_SYSTEM_LEFT_HANDED_Y_UP,
    // Left-Handed with Z axis pointing up and X forward. Used in Unreal Engine.
    COORDINATE_SYSTEM_LEFT_HANDED_Z_UP,
    // Right-Handed with Y pointing up and Z backward. Used in OpenGL.
    COORDINATE_SYSTEM_RIGHT_HANDED_Y_UP,
    // Right-Handed with Z pointing up and Y forward. Used in 3DSMax.
    COORDINATE_SYSTEM_RIGHT_HANDED_Z_UP,
    //< Right-Handed with Z pointing up and X forward. Used in ROS (REP 103).
    COORDINATE_SYSTEM_RIGHT_HANDED_Z_UP_X_FWD,
    COORDINATE_SYSTEM_LAST,
  };

  Coordinate();
  explicit Coordinate(CoordinateSystem coordinate_system);
  Coordinate(const Coordinate& other);
  Coordinate& operator=(const Coordinate& other);

  CoordinateSystem coordinate_system() const;

  template <typename T>
  Point3<T> Convert(const Point3<T>& point,
                    CoordinateSystem to_coordinate_system) const;
  template <typename T>
  Vector3<T> Convert(const Vector3<T>& vectord,
                     CoordinateSystem to_coordinate_system) const;

 private:
  CoordinateSystem coordinate_system_;
};

// clang-format off
constexpr struct {
  int x;
  int y;
  int z;
  int sign_x;
  int sign_y;
  int sign_z;
} const kConversionTable[] = {
    {0, 1, 2, 1, 1, 1}, {0, 1, 2, 1, -1, 1}, {2, 0, 1, 1, 1, -1}, {0, 1, 2, 1, -1, -1}, {0, 2, 1, 1, 1, -1}, {2, 0, 1, 1, -1, -1},
    {0, 1, 2, 1, -1, 1}, {0, 1, 2, 1, 1, 1}, {2, 0, 1, 1, 1, 1}, {0, 1, 2, 1, 1, -1}, {0, 2, 1, 1, 1, 1}, {2, 0, 1, 1, -1, 1},
    {1, 2, 0, 1, -1, 1}, {1, 2, 0, 1, 1, 1}, {0, 1, 2, 1, 1, 1}, {1, 2, 0, 1, 1, -1}, {1, 0, 2, 1, 1, 1}, {0, 1, 2, 1, -1, 1},
    {0, 1, 2, 1, -1, -1}, {0, 1, 2, 1, 1, -1}, {2, 0, 1, -1, 1, 1}, {0, 1, 2, 1, 1, 1}, {0, 2, 1, 1, -1, 1}, {2, 0, 1, -1, -1, 1},
    {0, 2, 1, 1, -1, 1}, {0, 2, 1, 1, 1, 1}, {1, 0, 2, 1, 1, 1}, {0, 2, 1, 1, 1, -1}, {0, 1, 2, 1, 1, 1}, {1, 0, 2, 1, -1, 1},
    {1, 2, 0, -1, -1, 1}, {1, 2, 0, -1, 1, 1}, {0, 1, 2, 1, -1, 1}, {1, 2, 0, -1, 1, -1}, {2, 1, 0, -1, 1, 1}, {0, 1, 2, 1, 1, 1},
};
// clang-format on

template <typename T>
Point3<T> Coordinate::Convert(const Point3<T>& point,
                              CoordinateSystem to_coordinate_system) const {
  T values[3] = {point.x(), point.y(), point.z()};

  size_t from_coord = static_cast<size_t>(coordinate_system_);
  size_t to_coord = static_cast<size_t>(to_coordinate_system);
  size_t size = static_cast<size_t>(COORDINATE_SYSTEM_LAST);

  auto& c = kConversionTable[size * from_coord + to_coord];

  return Point3<T>(values[c.x] * c.sign_x, values[c.y] * c.sign_y,
                   values[c.z] * c.sign_z);
};

template <typename T>
Vector3<T> Coordinate::Convert(const Vector3<T>& vector,
                               CoordinateSystem to_coordinate_system) const {
  T values[3] = {vector.x(), vector.y(), vector.z()};

  size_t from_coord = static_cast<size_t>(coordinate_system_);
  size_t to_coord = static_cast<size_t>(to_coordinate_system);
  size_t size = static_cast<size_t>(COORDINATE_SYSTEM_LAST);

  auto& c = kConversionTable[size * from_coord + to_coord];

  return Vector3<T>(values[c.x] * c.sign_x, values[c.y] * c.sign_y,
                    values[c.z] * c.sign_z);
}

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_COORDINATE_COORDINATE_H_