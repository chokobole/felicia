#include "felicia/core/lib/coordinate/coordinate.h"
#include "felicia/core/lib/unit/geometry/point.h"

#include "gtest/gtest.h"

namespace felicia {

void TestConversion(Coordinate::CoordinateSystem from_coordinate_system,
                    Coordinate::CoordinateSystem to_coordinate_system,
                    const Point3f& from_point, const Point3f& to_point) {
  Coordinate from_coordinate(from_coordinate_system);
  EXPECT_EQ(to_point,
            from_coordinate.Convert(from_point, to_coordinate_system));
  Coordinate to_coordinate(to_coordinate_system);
  EXPECT_EQ(from_point,
            to_coordinate.Convert(to_point, from_coordinate_system));
}

TEST(CoordinateTest, Conversion) {
  Point3f point(1, 2, 3);
  TestConversion(Coordinate::COORDINATE_SYSTEM_IMAGE,
                 Coordinate::COORDINATE_SYSTEM_IMAGE, point, Point3f(1, 2, 3));
  TestConversion(Coordinate::COORDINATE_SYSTEM_IMAGE,
                 Coordinate::COORDINATE_SYSTEM_LEFT_HANDED_Y_UP, point,
                 Point3f(1, -2, 3));
  TestConversion(Coordinate::COORDINATE_SYSTEM_IMAGE,
                 Coordinate::COORDINATE_SYSTEM_LEFT_HANDED_Z_UP, point,
                 Point3f(3, 1, -2));
  TestConversion(Coordinate::COORDINATE_SYSTEM_IMAGE,
                 Coordinate::COORDINATE_SYSTEM_RIGHT_HANDED_Y_UP, point,
                 Point3f(1, -2, -3));
  TestConversion(Coordinate::COORDINATE_SYSTEM_IMAGE,
                 Coordinate::COORDINATE_SYSTEM_RIGHT_HANDED_Z_UP, point,
                 Point3f(1, 3, -2));
  TestConversion(Coordinate::COORDINATE_SYSTEM_IMAGE,
                 Coordinate::COORDINATE_SYSTEM_RIGHT_HANDED_Z_UP_X_FWD, point,
                 Point3f(3, -1, -2));
}

}  // namespace felicia