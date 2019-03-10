#include "felicia/core/util/command_line_interface/flag_parser.h"

#include "gtest/gtest.h"

#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {
namespace {

#define PARSE_DELEGATE(BOOL, DelegateType, N, ...) \
  DelegateType delegate;                           \
  FlagParser parser;                               \
  parser.set_program_name("flag_parser_test");     \
  parser.mark_suppress_help();                     \
  const char* argv[N] = {__VA_ARGS__};             \
  EXPECT_##BOOL(parser.Parse(N, const_cast<char**>(argv), &delegate))

#define EXPECT_TRUE_PARSE_DELEGATE(DelegateType, N, ...) \
  PARSE_DELEGATE(TRUE, DelegateType, N + 1, "test", __VA_ARGS__)
#define EXPECT_FALSE_PARSE_DELEGATE(DelegateType, N, ...) \
  PARSE_DELEGATE(FALSE, DelegateType, N + 1, "test", __VA_ARGS__)

class ABoolFlag : public FlagParser::Delegate {
 public:
  ABoolFlag() {
    BoolFlag::Builder builder(MakeValueStore(&value_));
    auto flag = builder.SetShortName("-b").SetLongName("--bool").Build();
    flag_ = std::make_unique<BoolFlag>(flag);
  }

  bool value() const { return flag_->value(); }

  bool Parse(FlagParser& parser) override {
    return PARSE_OPTIONAL_FLAG(parser, flag_);
  }

  bool Validate() const override { return flag_->is_set(); }

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(flag_)

 protected:
  bool value_;
  std::unique_ptr<BoolFlag> flag_;
};

TEST(FlagParserTest, ParseBool) {
  { EXPECT_FALSE_PARSE_DELEGATE(ABoolFlag, 0); }

  {
    EXPECT_TRUE_PARSE_DELEGATE(ABoolFlag, 1, "-b");
    EXPECT_EQ(true, delegate.value());
  }

  {
    EXPECT_TRUE_PARSE_DELEGATE(ABoolFlag, 1, "-bool");
    EXPECT_EQ(true, delegate.value());
  }

  { EXPECT_FALSE_PARSE_DELEGATE(ABoolFlag, 1, "-a"); }

  { EXPECT_FALSE_PARSE_DELEGATE(ABoolFlag, 1, "--abc"); }
}

class AIntFlag : public FlagParser::Delegate {
 public:
  AIntFlag() {
    {
      IntFlag::Builder builder(MakeValueStore(&value_));
      auto flag = builder.SetName("int").Build();
      flag_ = std::make_unique<IntFlag>(flag);
    }
    {
      IntFlag::Builder builder(MakeValueStore(&value2_));
      auto flag = builder.SetShortName("-v").SetLongName("--value").Build();
      flag2_ = std::make_unique<IntFlag>(flag);
    }
  }

  int value() const { return flag_->value(); }
  int value2() const { return flag2_->value(); }

  bool Parse(FlagParser& parser) override {
    PARSE_POSITIONAL_FLAG(parser, 1, flag_);
    return PARSE_OPTIONAL_FLAG(parser, flag2_);
  }

  bool Validate() const override { return flag_->is_set(); };

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(flag_, flag2_)

 protected:
  int value_;
  std::unique_ptr<IntFlag> flag_;
  int value2_;
  std::unique_ptr<IntFlag> flag2_;
};

TEST(FlagParserTest, ParseInt) {
  { EXPECT_FALSE_PARSE_DELEGATE(AIntFlag, 0); }

  {
    EXPECT_TRUE_PARSE_DELEGATE(AIntFlag, 1, "123");
    EXPECT_EQ(123, delegate.value());
  }

  { EXPECT_FALSE_PARSE_DELEGATE(AIntFlag, 2, "-v", "123"); }

  { EXPECT_FALSE_PARSE_DELEGATE(AIntFlag, 3, "-v", "123", "456"); }

  { EXPECT_FALSE_PARSE_DELEGATE(AIntFlag, 3, "--value", "123", "456"); }

  {
    EXPECT_TRUE_PARSE_DELEGATE(AIntFlag, 3, "456", "-v", "123");
    EXPECT_EQ(456, delegate.value());
    EXPECT_EQ(123, delegate.value2());
  }

  {
    EXPECT_TRUE_PARSE_DELEGATE(AIntFlag, 2, "456", "-v=123");
    EXPECT_EQ(456, delegate.value());
    EXPECT_EQ(123, delegate.value2());
  }

  {
    EXPECT_TRUE_PARSE_DELEGATE(AIntFlag, 3, "456", "--value", "123");
    EXPECT_EQ(456, delegate.value());
    EXPECT_EQ(123, delegate.value2());
  }

  {
    EXPECT_TRUE_PARSE_DELEGATE(AIntFlag, 2, "456", "--value=123");
    EXPECT_EQ(456, delegate.value());
    EXPECT_EQ(123, delegate.value2());
  }
}

}  // namespace
}  // namespace felicia