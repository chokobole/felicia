#include "felicia/core/lib/file/csv_reader.h"

#include "gtest/gtest.h"

#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/lib/felicia_env.h"

namespace felicia {

TEST(CsvReaderTest, ReadRowsBasicTest) {
  CsvReader reader;
  base::FilePath path(
      FELICIA_RELATIVE_PATH("/felicia/core/lib/file/test/data.csv"));
  EXPECT_TRUE(reader.Open(path).ok());
  for (int i = 0; i < 2; ++i) {
    std::vector<std::string> rows;
    EXPECT_TRUE(reader.ReadRows(&rows));
    EXPECT_EQ(rows.size(), 5);
    for (int j = 0; j < 5; ++j) {
      EXPECT_EQ(base::NumberToString(i * 5 + j + 1), rows[j]);
    }
  }
}

}  // namespace felicia