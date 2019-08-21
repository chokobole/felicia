#include "felicia/core/lib/file/buffered_reader.h"

#include "gtest/gtest.h"

#include "felicia/core/lib/felicia_env.h"

namespace felicia {

TEST(BufferedReaderTest, OpenTest) {
  BufferedReader reader;
  base::FilePath path(
      FELICIA_RELATIVE_PATH("/felicia/core/lib/file/test/data.txt"));
  Status s = reader.Open(path);
  EXPECT_TRUE(s.ok());
  path = base::FilePath{
      FELICIA_RELATIVE_PATH("/felicia/core/lib/file/test/not-exist.txt")};
  s = reader.Open(path);
  EXPECT_FALSE(s.ok());
}

TEST(BufferedReaderTest, ReadLineBasicTest) {
  BufferedReader reader;
  base::FilePath path(
      FELICIA_RELATIVE_PATH("/felicia/core/lib/file/test/data.txt"));
  Status s = reader.Open(path);
  {
    std::string line;
    bool ret = reader.ReadLine(&line);
    EXPECT_TRUE(ret);
    EXPECT_EQ(line, "Hello\n");
  }
  {
    std::string line;
    bool ret = reader.ReadLine(&line);
    EXPECT_TRUE(ret);
    EXPECT_EQ(line, "World\n");
  }
}

TEST(BufferedReaderTest, ReadLineWithSmallBufferTest) {
  BufferedReader reader;
  base::FilePath path(
      FELICIA_RELATIVE_PATH("/felicia/core/lib/file/test/data.txt"));
  reader.SetBufferCapacityForTesting(2);
  Status s = reader.Open(path);
  {
    std::string line;
    bool ret = reader.ReadLine(&line);
    EXPECT_TRUE(ret);
    EXPECT_EQ(line, "Hello\n");
  }
  {
    std::string line;
    bool ret = reader.ReadLine(&line);
    EXPECT_TRUE(ret);
    EXPECT_EQ(line, "World\n");
  }
}

TEST(BufferedReaderTest, ReadLineNotEndingLineFeedTest) {
  BufferedReader reader;
  base::FilePath path(FELICIA_RELATIVE_PATH(
      "/felicia/core/lib/file/test/data-not-ending-with-linefeed.txt"));
  Status s = reader.Open(path);
  {
    std::string line;
    bool ret = reader.ReadLine(&line);
    EXPECT_TRUE(ret);
    EXPECT_EQ(line, "Hello\n");
  }
  {
    std::string line;
    bool ret = reader.ReadLine(&line);
    EXPECT_TRUE(ret);
    EXPECT_EQ(line, "World");
  }
}

}  // namespace felicia