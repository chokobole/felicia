#include "felicia/core/master/tool/node_flag_parser_delegate.h"

#include <iostream>

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

static const char* CREATE = "create";
static const char* GET = "get";

NodeFlagParserDelegate::NodeFlagParserDelegate()
    : current_command_(COMMAND_SELF) {
  {
    StringChoicesFlag::Builder builder(MakeValueStore<std::string>(
        &command_, "", Choices<std::string>{CREATE, GET}));
    auto flag = builder.SetName("COMMAND").Build();
    command_flag_ = std::make_unique<StringChoicesFlag>(flag);
  }
}

NodeFlagParserDelegate::~NodeFlagParserDelegate() = default;

bool NodeFlagParserDelegate::Parse(FlagParser& parser) {
  switch (current_command_) {
    case COMMAND_SELF:
      if (command_flag_->Parse(parser)) {
        if (strings::Equals(command_, CREATE)) {
          current_command_ = COMMAND_CREATE;
          parser.set_program_name(::base::StringPrintf(
              "%s %s", parser.program_name().c_str(), CREATE));
        } else if (strings::Equals(command_, GET)) {
          current_command_ = COMMAND_GET;
          parser.set_program_name(::base::StringPrintf(
              "%s %s", parser.program_name().c_str(), GET));
        }
        return true;
      }
      return false;
    case COMMAND_CREATE:
      return create_delegate_.Parse(parser);
    case COMMAND_GET:
      return get_delegate_.Parse(parser);
  }
}

std::vector<std::string> NodeFlagParserDelegate::CollectUsages() const {
  switch (current_command_) {
    case COMMAND_SELF:
      return {"COMMAND"};
    case COMMAND_CREATE:
      return create_delegate_.CollectUsages();
    case COMMAND_GET:
      return get_delegate_.CollectUsages();
  }
}

std::string NodeFlagParserDelegate::Description() const {
  switch (current_command_) {
    case COMMAND_SELF:
      return "Manage nodes";
    case COMMAND_CREATE:
      return create_delegate_.Description();
    case COMMAND_GET:
      return get_delegate_.Description();
  }
}

std::vector<NamedHelpType> NodeFlagParserDelegate::CollectNamedHelps() const {
  switch (current_command_) {
    case COMMAND_SELF: {
      return {
          std::make_pair(YELLOW_COLORED("Commands:"),
                         std::vector<std::string>{
                             MakeNamedHelpText(CREATE, "Create nodes"),
                             MakeNamedHelpText(GET, "Get nodes with filters"),
                         }),
      };
    }
    case COMMAND_CREATE:
      return create_delegate_.CollectNamedHelps();
    case COMMAND_GET:
      return get_delegate_.CollectNamedHelps();
  }
}

}  // namespace felicia