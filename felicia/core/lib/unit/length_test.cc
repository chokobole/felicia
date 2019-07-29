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

TEST(LengthTest, Operator) {
  const int64_t amount = 100;
  Length length = Length::FromMillimeter(amount);
  EXPECT_EQ(length * 2, Length(amount * 2));
  EXPECT_EQ(2 * length, Length(amount * 2));
  EXPECT_EQ(length * 3.742, Length(amount * 3.742));
  EXPECT_EQ(3.742 * length, Length(amount * 3.742));
  EXPECT_EQ(length * std::numeric_limits<int64_t>::max(), Length::Max());
  EXPECT_EQ(std::numeric_limits<int64_t>::max() * length, Length::Max());
  EXPECT_EQ(length * std::numeric_limits<int64_t>::min(), Length::Min());
  EXPECT_EQ(std::numeric_limits<int64_t>::min() * length, Length::Min());
  EXPECT_EQ(length / 2, Length(amount / 2));
  EXPECT_EQ(length / 2.5, Length(amount / 2.5));
  EXPECT_EQ(length / -2.5, Length(amount / -2.5));
  EXPECT_EQ(length / 0, Length::Max());
  EXPECT_EQ(length / Length::FromMillimeter(50), 2);
  const int64_t amount2 = 35;
  Length length2 = Length::FromMillimeter(amount2);
  EXPECT_EQ(length + length2, Length(amount + amount2));
  EXPECT_EQ(length + Length::Max(), Length::Max());
  EXPECT_EQ(length - length2, Length(amount - amount2));
  EXPECT_EQ(length2 - length, Length(amount2 - amount));
  EXPECT_EQ(length - Length::Max(),
            Length(amount - std::numeric_limits<int64_t>::max()));
}

}  // namespace felicia