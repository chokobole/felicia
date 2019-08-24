#include "felicia/core/lib/file/buffered_reader.h"

#include "gtest/gtest.h"

#include "felicia/core/lib/felicia_env.h"

#if BUILDFLAG(TRAVIS) && defined(OS_WIN)
#define NEWLINE "\r\n"
#else
#define NEWLINE "\n"
#endif

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
    EXPECT_TRUE(reader.ReadLine(&line));
    EXPECT_EQ(line, "Hello" NEWLINE);
  }
  {
    std::string line;
    EXPECT_TRUE(reader.ReadLine(&line));
    EXPECT_EQ(line, "World" NEWLINE);
  }
}

TEST(BufferedReaderTest, ReadLineWithRemoveCrOrLfTest) {
  BufferedReader reader(BufferedReader::REMOVE_CR_OR_LF);
  base::FilePath path(
      FELICIA_RELATIVE_PATH("/felicia/core/lib/file/test/data.txt"));
  Status s = reader.Open(path);
  {
    std::string line;
    EXPECT_TRUE(reader.ReadLine(&line));
    EXPECT_EQ(line, "Hello");
  }
  {
    std::string line;
    EXPECT_TRUE(reader.ReadLine(&line));
    EXPECT_EQ(line, "World");
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
    EXPECT_TRUE(reader.ReadLine(&line));
    EXPECT_EQ(line, "Hello" NEWLINE);
  }
  {
    std::string line;
    EXPECT_TRUE(reader.ReadLine(&line));
    EXPECT_EQ(line, "World" NEWLINE);
  }
}

TEST(BufferedReaderTest, ReadLineNotEndingLineFeedTest) {
  BufferedReader reader;
  base::FilePath path(FELICIA_RELATIVE_PATH(
      "/felicia/core/lib/file/test/data-not-ending-with-linefeed.txt"));
  Status s = reader.Open(path);
  {
    std::string line;
    EXPECT_TRUE(reader.ReadLine(&line));
    EXPECT_EQ(line, "Hello" NEWLINE);
  }
  {
    std::string line;
    EXPECT_TRUE(reader.ReadLine(&line));
    EXPECT_EQ(line, "World");
  }
}

}  // namespace felicia