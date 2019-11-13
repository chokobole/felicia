// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/unit/unit_helper.h"

#include "gtest/gtest.h"

namespace felicia {

TEST(UnitHelperTest, Operator) {
  const int64_t amount = 100;
  const int64_t max = std::numeric_limits<int64_t>::max();
  const int64_t min = std::numeric_limits<int64_t>::min();
  EXPECT_EQ(amount + 2, internal::SaturateAdd(amount, static_cast<int64_t>(2)));
  EXPECT_EQ(max, internal::SaturateAdd(max, max));
  EXPECT_EQ(min, internal::SaturateAdd(min, min));
  EXPECT_EQ(min + max, internal::SaturateAdd(min, max));
  EXPECT_EQ(max + min, internal::SaturateAdd(max, min));
  EXPECT_EQ(max, internal::SaturateAdd(amount, max));
  EXPECT_EQ(amount + min, internal::SaturateAdd(amount, min));
  EXPECT_EQ(max, internal::SaturateAdd(max, amount));
  EXPECT_EQ(min + amount, internal::SaturateAdd(min, amount));

  EXPECT_EQ(amount - 2, internal::SaturateSub(amount, static_cast<int64_t>(2)));
  EXPECT_EQ(0, internal::SaturateSub(max, max));
  EXPECT_EQ(0, internal::SaturateSub(min, min));
  EXPECT_EQ(min, internal::SaturateSub(min, max));
  EXPECT_EQ(max, internal::SaturateSub(max, min));
  EXPECT_EQ(amount - max, internal::SaturateSub(amount, max));
  EXPECT_EQ(max, internal::SaturateSub(amount, min));
  EXPECT_EQ(max - amount, internal::SaturateSub(max, amount));
  EXPECT_EQ(min, internal::SaturateSub(min, amount));

  EXPECT_EQ(amount * 2, internal::SaturateMul(amount, static_cast<int64_t>(2)));
  EXPECT_EQ(max, internal::SaturateMul(max, max));
  EXPECT_EQ(max, internal::SaturateMul(min, min));
  EXPECT_EQ(min, internal::SaturateMul(min, max));
  EXPECT_EQ(min, internal::SaturateMul(max, min));
  EXPECT_EQ(0, internal::SaturateMul(max, 0));
  EXPECT_EQ(0, internal::SaturateMul(0, max));
  EXPECT_EQ(0, internal::SaturateMul(min, 0));
  EXPECT_EQ(0, internal::SaturateMul(0, min));
  EXPECT_EQ(max, internal::SaturateMul(amount, max));
  EXPECT_EQ(min, internal::SaturateMul(amount, min));
  EXPECT_EQ(max, internal::SaturateMul(max, amount));
  EXPECT_EQ(min, internal::SaturateMul(min, amount));

  EXPECT_EQ(amount / 2, internal::SaturateDiv(amount, static_cast<int64_t>(2)));
  EXPECT_EQ(1, internal::SaturateDiv(max, max));
  EXPECT_EQ(1, internal::SaturateDiv(min, min));
  EXPECT_EQ(-1, internal::SaturateDiv(min, max));
  EXPECT_EQ(0, internal::SaturateDiv(max, min));
  EXPECT_EQ(0, internal::SaturateDiv(amount, max));
  EXPECT_EQ(0, internal::SaturateDiv(amount, min));
  EXPECT_EQ(max, internal::SaturateDiv(max, 0));
  EXPECT_EQ(0, internal::SaturateDiv(0, max));
  EXPECT_EQ(min, internal::SaturateDiv(min, 0));
  EXPECT_EQ(0, internal::SaturateDiv(0, min));
  EXPECT_EQ(max / amount, internal::SaturateDiv(max, amount));
  EXPECT_EQ(min / amount, internal::SaturateDiv(min, amount));
}

}  // namespace felicia