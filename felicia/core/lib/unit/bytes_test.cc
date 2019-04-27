#include "felicia/core/lib/unit/bytes.h"

#include "gtest/gtest.h"

namespace felicia {

TEST(BytesTest, Construct) {
  const size_t amount = 100;
  Bytes bytes = Bytes::FromBytes(amount * Bytes::kGigaBytes);
  Bytes killo_bytes = Bytes::FromKilloBytes(amount * Bytes::kMegaBytes);
  Bytes mega_bytes = Bytes::FromMegaBytes(amount * Bytes::kKilloBytes);
  Bytes giga_bytes = Bytes::FromGigaBytes(amount);
  ASSERT_EQ(bytes, killo_bytes);
  ASSERT_EQ(bytes, mega_bytes);
  ASSERT_EQ(bytes, giga_bytes);

  const double amountd = 3.35;
  Bytes bytesd = Bytes::FromBytes(amountd * Bytes::kGigaBytes);
  Bytes killo_bytesd = Bytes::FromKilloBytesD(amountd * Bytes::kMegaBytes);
  Bytes mega_bytesd = Bytes::FromMegaBytesD(amountd * Bytes::kKilloBytes);
  Bytes giga_bytesd = Bytes::FromGigaBytesD(amountd);
  ASSERT_EQ(bytesd, killo_bytesd);
  ASSERT_EQ(bytesd, mega_bytesd);
  ASSERT_EQ(bytesd, giga_bytesd);

  ASSERT_EQ(Bytes::FromKilloBytes(std::numeric_limits<size_t>::max()),
            Bytes::Max());
  ASSERT_EQ(Bytes::FromKilloBytesD(std::numeric_limits<size_t>::max()),
            Bytes::Max());
}

TEST(BytesTest, Operator) {
  const size_t amount = 100;
  Bytes bytes = Bytes::FromBytes(amount);
  ASSERT_EQ(bytes * 2, Bytes(amount * 2));
  ASSERT_EQ(2 * bytes, Bytes(amount * 2));
  ASSERT_EQ(bytes * 3.742, Bytes(amount * 3.742));
  ASSERT_EQ(3.742 * bytes, Bytes(amount * 3.742));
  ASSERT_EQ(bytes * std::numeric_limits<size_t>::max(), Bytes::Max());
  ASSERT_EQ(std::numeric_limits<size_t>::max() * bytes, Bytes::Max());
  ASSERT_EQ(bytes / 2, Bytes(amount / 2));
  ASSERT_EQ(bytes / 2.5, Bytes(amount / 2.5));
  ASSERT_EQ(bytes / -2.5, Bytes());
  ASSERT_EQ(bytes / 0, Bytes());
  ASSERT_EQ(bytes / Bytes::FromBytes(50), 2);
  const size_t amount2 = 35;
  Bytes bytes2 = Bytes::FromBytes(amount2);
  ASSERT_EQ(bytes + bytes2, Bytes(amount + amount2));
  ASSERT_EQ(bytes + Bytes::Max(), Bytes::Max());
  ASSERT_EQ(bytes - bytes2, Bytes(amount - amount2));
  ASSERT_EQ(bytes2 - bytes, Bytes());
  ASSERT_EQ(bytes - Bytes::Max(), Bytes());
}

}  // namespace felicia