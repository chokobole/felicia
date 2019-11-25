// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/unit/length.h"

#include "gtest/gtest.h"

namespace felicia {

TEST(LengthTest, Construct) {
  const int64_t amount = 120;
  Length millimeter = Length::FromMillimeter(amount * Length::kKillometer);
  Length centimeter = Length::FromCentimeter(amount * Length::kKillometer / 10);
  Length meter = Length::FromMeter(amount * Length::kKillometer / 1000);
  Length killometer = Length::FromKillometer(amount);
  EXPECT_EQ(millimeter, centimeter);
  EXPECT_EQ(millimeter, meter);
  EXPECT_EQ(millimeter, killometer);

  Length inch = Length::FromInch(amount);
  Length feet = Length::FromFeet(amount);
  EXPECT_LE((feet - (inch * 12)).InMillimeter(), 1);

  const double amountd = 3.35;
  Length centimeterd =
      Length::FromCentimeterD(amountd * Length::kKillometer / 10);
  Length meterd = Length::FromMeterD(amountd * Length::kKillometer / 1000);
  Length killometerd = Length::FromKillometerD(amountd);
  EXPECT_EQ(centimeterd, meterd);
  EXPECT_EQ(centimeterd, killometerd);

  EXPECT_EQ(Length::FromKillometer(std::numeric_limits<int64_t>::max()),
            Length::Max());
  EXPECT_EQ(Length::FromKillometerD(std::numeric_limits<int64_t>::max()),
            Length::Max());
}

}  // namespace felicia