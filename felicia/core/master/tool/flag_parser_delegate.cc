#include "felicia/core/master/tool/flag_parser_delegate.h"

#include <iostream>
#include <sstream>

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

static const char* NODE = "node";

FlagParserDelegate::FlagParserDelegate() : current_command_(COMMAND_SELF) {
  {
    StringChoicesFlag::Builder builder(
        MakeValueStore<std::string>(&command_, "", Choices<std::string>{NODE}));
    auto flag = builder.SetName("COMMAND").Build();
    command_flag_ = std::make_unique<StringChoicesFlag>(flag);
  }
}

FlagParserDelegate::~FlagParserDelegate() = default;

bool FlagParserDelegate::Parse(FlagParser& parser) {
  switch (current_command_) {
    case COMMAND_SELF:
      if (command_flag_->Parse(parser)) {
        if (strings::Equals(command_, NODE)) {
          current_command_ = COMMAND_NODE;
          parser.set_program_name(::base::StringPrintf(
              "%s %s", parser.program_name().c_str(), NODE));
        }
        return true;
      }
      return false;
    case COMMAND_NODE:
      return node_delegate_.Parse(parser);
  }
}

std::vector<std::string> FlagParserDelegate::CollectUsages() const {
  switch (current_command_) {
    case COMMAND_SELF:
      return {"[OPTIONS]", "COMMAND"};
    case COMMAND_NODE:
      return node_delegate_.CollectUsages();
  }
}

std::string FlagParserDelegate::Description() const {
  switch (current_command_) {
    case COMMAND_SELF:
      return "A proxy command to manage felicia";
    case COMMAND_NODE:
      return node_delegate_.Description();
  }
}

std::vector<NamedHelpType> FlagParserDelegate::CollectNamedHelps() const {
  switch (current_command_) {
    case COMMAND_SELF: {
      return {
          std::make_pair(YELLOW_COLORED("Management commands:"),
                         std::vector<std::string>{
                             MakeNamedHelpText(NODE, "Manage nodes"),
                         }),
      };
    }
    case COMMAND_NODE:
      return node_delegate_.CollectNamedHelps();
  }
}

}  // namespace felicia