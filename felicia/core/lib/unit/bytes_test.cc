#include "felicia/core/lib/unit/bytes.h"

#include "gtest/gtest.h"

namespace felicia {

TEST(BytesTest, Construct) {
  const int64_t amount = 100;
  Bytes bytes = Bytes::FromBytes(amount * Bytes::kGigaBytes);
  Bytes killo_bytes = Bytes::FromKilloBytes(amount * Bytes::kMegaBytes);
  Bytes mega_bytes = Bytes::FromMegaBytes(amount * Bytes::kKilloBytes);
  Bytes giga_bytes = Bytes::FromGigaBytes(amount);
  EXPECT_EQ(bytes, killo_bytes);
  EXPECT_EQ(bytes, mega_bytes);
  EXPECT_EQ(bytes, giga_bytes);

  const double amountd = 3.35;
  Bytes bytesd = Bytes::FromBytes(amountd * Bytes::kGigaBytes);
  Bytes killo_bytesd = Bytes::FromKilloBytesD(amountd * Bytes::kMegaBytes);
  Bytes mega_bytesd = Bytes::FromMegaBytesD(amountd * Bytes::kKilloBytes);
  Bytes giga_bytesd = Bytes::FromGigaBytesD(amountd);
  EXPECT_EQ(bytesd, killo_bytesd);
  EXPECT_EQ(bytesd, mega_bytesd);
  EXPECT_EQ(bytesd, giga_bytesd);

  EXPECT_EQ(Bytes::FromKilloBytes(std::numeric_limits<int64_t>::max()),
            Bytes::Max());
  EXPECT_EQ(Bytes::FromKilloBytesD(std::numeric_limits<int64_t>::max()),
            Bytes::Max());
}

TEST(BytesTest, Operator) {
  const int64_t amount = 100;
  Bytes bytes = Bytes::FromBytes(amount);
  EXPECT_EQ(bytes * 2, Bytes(amount * 2));
  EXPECT_EQ(2 * bytes, Bytes(amount * 2));
  EXPECT_EQ(bytes * 3.742, Bytes(amount * 3.742));
  EXPECT_EQ(3.742 * bytes, Bytes(amount * 3.742));
  EXPECT_EQ(bytes * std::numeric_limits<int64_t>::max(), Bytes::Max());
  EXPECT_EQ(std::numeric_limits<int64_t>::max() * bytes, Bytes::Max());
  EXPECT_EQ(bytes / 2, Bytes(amount / 2));
  EXPECT_EQ(bytes / 2.5, Bytes(amount / 2.5));
  EXPECT_EQ(bytes / -2.5, Bytes(amount / -2.5));
  EXPECT_EQ(bytes / 0, Bytes());
  EXPECT_EQ(bytes / Bytes::FromBytes(50), 2);
  const int64_t amount2 = 35;
  Bytes bytes2 = Bytes::FromBytes(amount2);
  EXPECT_EQ(bytes + bytes2, Bytes(amount + amount2));
  EXPECT_EQ(bytes + Bytes::Max(), Bytes::Max());
  EXPECT_EQ(bytes - bytes2, Bytes(amount - amount2));
  EXPECT_EQ(bytes2 - bytes, Bytes(amount2 - amount));
  EXPECT_EQ(bytes - Bytes::Max(),
            Bytes(amount - std::numeric_limits<int64_t>::max()));
}

}  // namespace felicia