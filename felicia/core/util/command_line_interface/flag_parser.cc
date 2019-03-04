#include "felicia/core/util/command_line_interface/flag_parser.h"

#include <iostream>

#include "third_party/chromium/base/logging.h"

#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/util/command_line_interface/flag.h"
#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

bool FlagParser::Parse(int argc, char** argv, Delegate* delegate) {
  current_idx_ = 1;
  argc_ = argc;
  argv_ = argv;

  while (current_idx_ < argc_) {
    if (strings::Equals(current(), "--help")) {
      if (!suppress_help_) PrintHelp(delegate);
      return false;
    }
    if (!delegate->Parse(*this)) {
      std::cerr << TextStyle::Red("Error: ") << current() << " is not valid"
                << std::endl;
      if (!suppress_help_) PrintHelp(delegate);
      return false;
    }
    Proceed();
  }

  if (!delegate->Validate()) {
    if (!suppress_help_) PrintHelp(delegate);
    return false;
  }

  return true;
}

void FlagParser::Proceed() {
  if (current_idx_ < argc_) current_idx_++;
}

void FlagParser::PrintUsage(Delegate* delegate) {
  auto usages = delegate->CollectUsages();

  std::cerr << TextStyle::Green("Usage: ") << std::endl;
  std::cerr << program_name_;
  const int line_width = 50;
  size_t flag_start = program_name_.length();
  int remain_len = line_width - flag_start;
  const size_t size = usages.size();
  for (size_t i = 0; i < size; i++) {
    if (remain_len < 0) {
      std::cerr << std::endl;
      std::cerr << std::string(flag_start, ' ');
      remain_len = line_width;
    }
    APPEND_AND_DECREASE_LENGTH(std::cerr, " ", remain_len);
    APPEND_AND_DECREASE_LENGTH(std::cerr, usages[i], remain_len);
  }
  std::cerr << std::endl;
}

void FlagParser::PrintDescription(Delegate* delegate) {
  auto description = delegate->Description();
  if (!description.empty()) std::cerr << std::endl << description << std::endl;
}

void FlagParser::PrintHelp(Delegate* delegate) {
  PrintUsage(delegate);

  PrintDescription(delegate);

  auto named_helps = delegate->CollectNamedHelps();

  for (auto& named_help : named_helps) {
    if (named_help.second.size() == 0) continue;
    std::cerr << std::endl << named_help.first << std::endl;
    for (auto& flag_help : named_help.second) {
      std::cerr << flag_help << std::endl;
    }
  }
}

}  // namespace felicia