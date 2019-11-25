// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/base/choices.h"

#include <string>

#include "gtest/gtest.h"

namespace felicia {
namespace {

TEST(ChoicesTest, Basic) {
  Choices<std::string> animals("bird", "dog", "cat");
  EXPECT_FALSE(animals.In("lion"));
  for (auto animal : animals.values()) {
    EXPECT_TRUE(animals.In(animal));
  }
}

}  // namespace
}  // namespace felicia