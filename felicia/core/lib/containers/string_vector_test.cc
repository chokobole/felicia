#include "felicia/core/lib/containers/string_vector.h"

#include "gtest/gtest.h"

namespace felicia {

TEST(StringVectorTest, Push) {
  StringVector vector;
  struct Point {
    float x;
    float y;

    Point(float x, float y) : x(x), y(y) {}
  };
  Point p(1, 2);
  Point p2(3, 4);
  vector.push_back(p);
  vector.push_back(p2);
  ASSERT_EQ(vector.size<Point>(), 2);

  Point& p3 = vector.at<Point>(0);
  ASSERT_EQ(p.x, p3.x);
  ASSERT_EQ(p.y, p3.y);
  Point& p4 = vector.at<Point>(1);
  ASSERT_EQ(p2.x, p4.x);
  ASSERT_EQ(p2.y, p4.y);

  p3.x = 5;
  ASSERT_EQ(p3.x, 5);
  ASSERT_EQ(vector.at<Point>(0).x, 5);
}

}  // namespace felicia