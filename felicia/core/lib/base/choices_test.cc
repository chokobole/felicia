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