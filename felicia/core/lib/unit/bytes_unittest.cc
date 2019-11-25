// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

}  // namespace felicia