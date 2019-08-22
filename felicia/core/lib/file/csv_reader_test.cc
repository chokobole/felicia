#include "felicia/core/lib/file/csv_reader.h"

#include "gtest/gtest.h"

#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/lib/felicia_env.h"

namespace felicia {

TEST(CsvReaderTest, ReadItemsBasicTest) {
  CsvReader reader;
  base::FilePath path(
      FELICIA_RELATIVE_PATH("/felicia/core/lib/file/test/data.csv"));
  Status s = reader.Open(path);
  {
    std::vector<std::string> items;
    bool ret = reader.ReadItems(&items);
    EXPECT_TRUE(ret);
    EXPECT_EQ(items.size(), 5);
    for (int i = 0; i < 5; ++i) {
      EXPECT_EQ(base::NumberToString(i + 1), items[i]);
    }
  }
  {
    std::vector<std::string> items;
    bool ret = reader.ReadItems(&items);
    EXPECT_TRUE(ret);
    EXPECT_EQ(items.size(), 5);
    for (int i = 0; i < 5; ++i) {
      EXPECT_EQ(base::NumberToString(i + 6), items[i]);
    }
  }
}

}  // namespace felicia