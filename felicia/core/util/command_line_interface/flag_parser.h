// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_FLAG_PARSER_H_
#define FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_FLAG_PARSER_H_

#include <string>
#include <utility>
#include <vector>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

typedef std::pair<std::string, std::vector<std::string>> NamedHelpType;

class FEL_EXPORT FlagParser {
 public:
  // The Parse method is called when parser meet every argument which is
  // tokenized per space.
  //
  // Flag<T> is designed to have no default constructor. Becuase there's a
  // chance to be parsed without having values which are needed in Range<T>
  // and Choices<T>. Therefore every Flag<T> should be passed from outside to
  // Delegate.
  //
  // Typical Flag looks like below.
  //
  // class FooBarFlag : public FlagParser::Delegate {
  //  public:
  //   FooBarFlag() {
  //     {
  //       IntFlag::Builder builder(MakeValueStore(&foo_));
  //       auto flag = builder.SetName("foo")
  //                          .SetHelp("foo value").Build();
  //       foo_flag_ = std::make_unique<IntFlag>(flag);
  //     }
  //
  //     {
  //       StringDefaultFlag::Builder builder(MakeValueStore(&bar_, "bar"));
  //       auto flag = builder.SetShortName("-b").SetLongName("--bar")
  //                          .SetHelp("foo value").Build();
  //       bar_flag_ = std::make_unique<StringDefaultFlag>(flag);
  //     }
  //   }
  //
  //   bool Parse(FlagParser& parser) override {
  //     PARSE_POSITIONAL_FLAG(parser, 1, foo_flag_);
  //     return PARSE_OPTIONAL_FLAG(parser, bar_flag_);
  //   }
  //
  //   bool Validate() const override {
  //     return foo_flag_->is_set();
  //   }
  //
  //   // This automatically defines methods which are used to be printed out
  //   // help message when needed.
  //   AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(foo_flag_, bar_flag_)
  //
  //  private:
  //   int foo_;
  //   std::string bar_
  //   std::unique_ptr<IntFlag> foo_flag_;
  //   std::unique_ptr<StringDefaultFlag> bar_flag_;
  // }
  class FEL_EXPORT Delegate {
   public:
    Delegate();
    virtual ~Delegate();

    virtual bool Parse(FlagParser& parser) = 0;

    virtual bool Validate() const;

    virtual std::vector<std::string> CollectUsages() const;
    virtual std::string Description() const;
    virtual std::vector<NamedHelpType> CollectNamedHelps() const;

   protected:
    void PreParse();

    template <int N = 1, typename T>
    bool ParsePositionalFlag(FlagParser& parser, T&& arg) {
      return arg->Parse(parser);
    }

    template <int N = 1, typename T, typename... Rest>
    bool ParsePositionalFlag(FlagParser& parser, T&& arg, Rest&&... rest) {
      if (parser_index_ == N) {
        return ParsePositionalFlag<N>(parser, std::forward<T>(arg));
      }
      return ParsePositionalFlag<N + 1>(parser, std::forward<Rest>(rest)...);
    }

    template <typename T>
    bool ParseOptionalFlag(FlagParser& parser, T&& arg) {
      return arg->Parse(parser);
    }

    template <typename T, typename... Rest>
    bool ParseOptionalFlag(FlagParser& parser, T&& arg, Rest&&... rest) {
      return ParseOptionalFlag(parser, std::forward<T>(arg)) ||
             ParseOptionalFlag(parser, std::forward<Rest>(rest)...);
    }

    int parser_index_;
  };

  FlagParser();
  ~FlagParser();

  // Set program name to display.
  // For example, when there are --foo, --bar flags and type --help,
  // It shows |program_name| [--foo] [--bar]
  void set_program_name(const std::string& program_name);
  const std::string& program_name() const;
  void mark_suppress_help();
  // Parse by passing every each |argv| to |delegate|.
  bool Parse(int argc, char** argv, Delegate* delegate);

 private:
  template <typename T, typename Traits>
  friend class Flag;

  void PrintUsage(Delegate* delegate);
  void PrintDescription(Delegate* delegate);
  void PrintHelp(Delegate* delegate);

  base::StringPiece current();
  void Proceed();

  std::string program_name_;
  bool suppress_help_;
  int argc_;
  int current_idx_;
  char** argv_;

  DISALLOW_COPY_AND_ASSIGN(FlagParser);
};

template <typename T>
void AddUsage(std::vector<std::string>& usages, T&& flag) {
  usages.push_back(std::forward<T>(flag)->usage());
}

template <typename T, typename... Rest>
void AddUsage(std::vector<std::string>& usages, T&& flag, Rest&&... rest) {
  AddUsage(usages, std::forward<T>(flag));
  AddUsage(usages, std::forward<Rest>(rest)...);
}

template <typename T>
void AddHelpIfPositional(std::vector<std::string>& helps, T&& flag) {
  if (flag->is_positional()) {
    helps.push_back(std::forward<T>(flag)->help());
  }
}

template <typename T, typename... Rest>
void AddHelpIfPositional(std::vector<std::string>& helps, T&& flag,
                         Rest&&... rest) {
  AddHelpIfPositional(helps, std::forward<T>(flag));
  AddHelpIfPositional(helps, std::forward<Rest>(rest)...);
}

template <typename T>
void AddHelpIfOptional(std::vector<std::string>& helps, T&& flag) {
  if (flag->is_optional()) {
    helps.push_back(std::forward<T>(flag)->help());
  }
}

template <typename T, typename... Rest>
void AddHelpIfOptional(std::vector<std::string>& helps, T&& flag,
                       Rest&&... rest) {
  AddHelpIfOptional(helps, std::forward<T>(flag));
  AddHelpIfOptional(helps, std::forward<Rest>(rest)...);
}

#define AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(...)                         \
  std::vector<std::string> CollectUsages() const override {                  \
    std::vector<std::string> usages;                                         \
    usages.push_back("[--help]");                                            \
    AddUsage(usages, __VA_ARGS__);                                           \
    return usages;                                                           \
  }                                                                          \
  std::vector<NamedHelpType> CollectNamedHelps() const override {            \
    std::vector<std::string> positional_helps;                               \
    AddHelpIfPositional(positional_helps, __VA_ARGS__);                      \
    std::vector<std::string> optional_helps;                                 \
    AddHelpIfOptional(optional_helps, __VA_ARGS__);                          \
    std::vector<NamedHelpType> helps;                                        \
    helps.push_back(std::make_pair(TextStyle::Blue("Positional arguments:"), \
                                   std::move(positional_helps)));            \
    helps.push_back(std::make_pair(TextStyle::Yellow("Optional arguments:"), \
                                   std::move(optional_helps)));              \
    return helps;                                                            \
  }

#define PARSE_POSITIONAL_FLAG(paresr, N, ...)        \
  PreParse();                                        \
  if (parser_index_ <= N) {                          \
    return ParsePositionalFlag(parser, __VA_ARGS__); \
  }

#define PARSE_OPTIONAL_FLAG(paresr, ...) ParseOptionalFlag(parser, __VA_ARGS__)

}  // namespace felicia

#endif  // FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_FLAG_PARSER_H_