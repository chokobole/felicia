// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/util/command_line_interface/flag.h"

#include "gtest/gtest.h"

namespace felicia {

TEST(FlagTest, DefaultValue) {
  {
    bool value;
    BoolDefaultFlag::Builder builder(MakeValueStore(&value, true));
    BoolDefaultFlag flag = builder.SetShortName("-a").Build();
    EXPECT_EQ(true, flag.value());
    EXPECT_TRUE(flag.set_value(false));
    EXPECT_EQ(false, flag.value());
    EXPECT_EQ(false, value);
  }

  {
    int value;
    IntDefaultFlag::Builder builder(MakeValueStore(&value, 32));
    IntDefaultFlag flag = builder.SetShortName("-a").Build();
    EXPECT_EQ(32, flag.value());
    EXPECT_TRUE(flag.set_value(64));
    EXPECT_EQ(64, flag.value());
    EXPECT_EQ(64, value);
  }
}

TEST(FlagTest, Is) {
  {
    bool value;
    BoolFlag::Builder builder(MakeValueStore(&value));
    BoolFlag flag = builder.SetShortName("-a").Build();
    EXPECT_TRUE(flag.Is<bool>());
    EXPECT_FALSE(flag.Is<int>());
  }

  {
    int value;
    IntFlag::Builder builder(MakeValueStore(&value));
    IntFlag flag = builder.SetShortName("-a").Build();
    EXPECT_FALSE(flag.Is<bool>());
    EXPECT_TRUE(flag.Is<int>());
  }
}

TEST(FlagTest, NameRule) {
  {
    bool value;
    BoolFlag::Builder builder(MakeValueStore(&value));
    EXPECT_DEATH_IF_SUPPORTED(builder.Build(), "");
  }
  {
    bool value;
    BoolFlag::Builder builder(MakeValueStore(&value));
    EXPECT_DEATH_IF_SUPPORTED(
        builder.SetName("bool").SetShortName("-b").Build(), "");
  }
  {
    bool value;
    BoolFlag::Builder builder(MakeValueStore(&value));
    EXPECT_DEATH_IF_SUPPORTED(
        builder.SetName("bool").SetLongName("--bool").Build(), "");
  }
}

TEST(FlagTest, Range) {
  {
    int min = 1;
    int max = 6;
    Range<int> dices(min, max);
    int value;
    IntRangeFlag::Builder builder(MakeValueStore(&value, 1, dices));
    IntRangeFlag flag = builder.SetShortName("-a").Build();
    EXPECT_FALSE(flag.Is<bool>());
    EXPECT_TRUE(flag.Is<int>());

    EXPECT_FALSE(flag.set_value(min - 1));
    EXPECT_FALSE(flag.set_value(max + 1));
    for (int i = min; i <= max; i++) {
      EXPECT_TRUE(flag.set_value(i));
      EXPECT_EQ(i, flag.value());
      EXPECT_EQ(i, value);
    }
  }
}

TEST(FlagTest, Choices) {
  {
    Choices<std::string> animals("bird", "dog", "cat");
    std::string value;
    StringChoicesFlag::Builder builder(
        MakeValueStore<std::string>(&value, "bird", animals));
    StringChoicesFlag flag = builder.SetShortName("-a").Build();
    EXPECT_FALSE(flag.Is<bool>());
    EXPECT_TRUE(flag.Is<std::string>());

    EXPECT_FALSE(flag.set_value("lion"));
    for (auto animal : animals.values()) {
      EXPECT_TRUE(flag.set_value(animal));
      EXPECT_EQ(0, animal.compare(flag.value()));
      EXPECT_EQ(0, animal.compare(value));
    }
  }

  {
    Choices<int> odds(1, 3, 5);
    int value;
    IntChoicesFlag::Builder builder(MakeValueStore(&value, 1, odds));
    IntChoicesFlag flag = builder.SetShortName("-a").Build();
    EXPECT_FALSE(flag.Is<std::string>());
    EXPECT_TRUE(flag.Is<int>());

    EXPECT_FALSE(flag.set_value(2));
    for (auto odd : odds.values()) {
      EXPECT_TRUE(flag.set_value(odd));
      EXPECT_EQ(odd, flag.value());
      EXPECT_EQ(odd, value);
    }
  }
}

}  // namespace felicia