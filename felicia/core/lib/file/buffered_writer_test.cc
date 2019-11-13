// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/file/buffered_writer.h"

#include "gtest/gtest.h"

#include "felicia/core/lib/file/buffered_reader.h"

namespace felicia {

TEST(BufferedWriterTest, WriteLineBasicTest) {
  BufferedWriter writer;
  base::FilePath path(FILE_PATH_LITERAL("test.txt"));
  Status s = writer.Open(path);
  EXPECT_TRUE(writer.WriteLine("Hello"));
  EXPECT_TRUE(writer.WriteLine("World"));
  writer.Close();

  BufferedReader reader(BufferedReader::REMOVE_CR_OR_LF);
  EXPECT_TRUE(reader.Open(path).ok());
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

TEST(BufferedWriterTest, WriteLineWithSmallBufferTest) {
  BufferedWriter writer;
  base::FilePath path(FILE_PATH_LITERAL("test.txt"));
  writer.SetBufferCapacityForTesting(2);
  Status s = writer.Open(path);
  EXPECT_TRUE(writer.WriteLine("Hello"));
  EXPECT_TRUE(writer.WriteLine("World"));
  writer.Close();

  BufferedReader reader(BufferedReader::REMOVE_CR_OR_LF);
  EXPECT_TRUE(reader.Open(path).ok());
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

}  // namespace felicia