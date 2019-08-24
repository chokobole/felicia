#include "felicia/core/lib/file/csv_writer.h"

#include "gtest/gtest.h"

#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/lib/file/csv_reader.h"

namespace felicia {

TEST(CsvWriterTest, WriteRowsBasicTest) {
  CsvWriter writer;
  base::FilePath path(FILE_PATH_LITERAL("test.csv"));
  Status s = writer.Open(path);
  {
    std::vector<std::string> rows{"1", "2", "3", "4", "5"};
    EXPECT_TRUE(writer.WriteRows(rows));
  }
  {
    std::vector<std::string> rows{"6", "7", "8", "9", "10"};
    EXPECT_TRUE(writer.WriteRows(rows));
  }
  writer.Close();

  CsvReader reader;
  reader.Open(path);
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