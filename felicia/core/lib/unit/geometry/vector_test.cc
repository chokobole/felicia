#include "felicia/core/lib/unit/geometry/vector.h"

#include "gtest/gtest.h"

namespace felicia {

TEST(VectorTest, BasicOperation) {
  Vectorf vector(2, 3);
  Vectorf vector2(4, 5);
  EXPECT_EQ(2, vector.x());
  EXPECT_EQ(3, vector.y());
  EXPECT_EQ(vector, Vectorf(2, 3));
  EXPECT_NE(vector, vector2);

  EXPECT_EQ(std::sqrt(13.0), vector.Norm());
  EXPECT_EQ(13.0, vector.SquaredNorm());
  EXPECT_EQ(vector.Scale(1 / vector.SquaredNorm()), vector.Normalize());
}

TEST(Vector3Test, BasicOperation) {
  Vector3f vector(2, 3, 5);
  Vector3f vector2(4, 5, 7);
  EXPECT_EQ(2, vector.x());
  EXPECT_EQ(3, vector.y());
  EXPECT_EQ(5, vector.z());
  EXPECT_EQ(vector, Vector3f(2, 3, 5));
  EXPECT_NE(vector, vector2);

  EXPECT_EQ(std::sqrt(38.0), vector.Norm());
  EXPECT_EQ(38.0, vector.SquaredNorm());
  EXPECT_EQ(vector.Scale(1 / vector.SquaredNorm()), vector.Normalize());
}

}  // namespace felicia