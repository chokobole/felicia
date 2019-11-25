// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/unit/geometry/point.h"

#include "gtest/gtest.h"

namespace felicia {

TEST(PointTest, BasicOperation) {
  Pointf point(2, 3);
  Pointf point2(4, 5);
  EXPECT_EQ(2, point.x());
  EXPECT_EQ(3, point.y());
  EXPECT_EQ(point, Pointf(2, 3));
  EXPECT_NE(point, point2);

  Vectorf vec(1, 3);
  EXPECT_EQ(std::sqrt(8.0), point.Distance(point2));
  EXPECT_EQ(Pointf(3, 6), point.Translate(vec));
  EXPECT_EQ(Pointf(3, 6), point + vec);
  EXPECT_EQ(Pointf(6, 9), point.Scale(3));
  EXPECT_EQ(Pointf(6, 9), point * 3);
  EXPECT_EQ(Pointf(6, 9), 3 * point);
}

TEST(Point3Test, BasicOperation) {
  Point3f point(2, 3, 5);
  Point3f point2(4, 5, 7);
  EXPECT_EQ(2, point.x());
  EXPECT_EQ(3, point.y());
  EXPECT_EQ(5, point.z());
  EXPECT_EQ(point, Point3f(2, 3, 5));
  EXPECT_NE(point, point2);

  Vector3f vec(1, 3, 7);
  EXPECT_EQ(std::sqrt(12.0), point.Distance(point2));
  EXPECT_EQ(Point3f(3, 6, 12), point.Translate(vec));
  EXPECT_EQ(Point3f(3, 6, 12), point + vec);
  EXPECT_EQ(Point3f(6, 9, 15), point.Scale(3));
  EXPECT_EQ(Point3f(6, 9, 15), point * 3);
  EXPECT_EQ(Point3f(6, 9, 15), 3 * point);
}

}  // namespace felicia