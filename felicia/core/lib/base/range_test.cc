// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/base/range.h"

#include "gtest/gtest.h"

namespace felicia {
namespace {

TEST(RangeTest, Basic) {
  int min = 1;
  int max = 6;
  Range<int> range(min, max);

  EXPECT_FALSE(range.In(min - 1));
  EXPECT_FALSE(range.In(max + 1));
  for (int i = min; i <= max; i++) {
    EXPECT_TRUE(range.In(i));
  }
}

}  // namespace
}  // namespace felicia