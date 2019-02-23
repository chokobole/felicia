#include "felicia/core/master/tool/node_create_flag_parser_delegate.h"

#include <iostream>

#include "third_party/chromium/net/base/ip_address.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

NodeCreateFlagParserDelegate::NodeCreateFlagParserDelegate() {
  {
    StringFlag::Builder builder(MakeValueStore(&name_));
    auto flag = builder.SetLongName("--name").SetHelp("name").Build();
    name_flag_ = std::make_unique<StringFlag>(flag);
  }

  {
    IntFlag::Builder builder(MakeValueStore(&port_));
    auto flag = builder.SetLongName("--port").SetHelp("port number").Build();
    port_flag_ = std::make_unique<IntFlag>(flag);
  }
}

NodeCreateFlagParserDelegate::~NodeCreateFlagParserDelegate() = default;

bool NodeCreateFlagParserDelegate::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, name_flag_, port_flag_);
}

bool NodeCreateFlagParserDelegate::Validate() {
  if (port_flag_->is_set() && !(1024 <= port_ && port_ <= 65535)) {
    std::cerr << RED_COLORED("Error: ") << "port is not in 1024 ~ 65535"
              << std::endl;
    return false;
  }

  return true;
}

std::vector<std::string> NodeCreateFlagParserDelegate::CollectUsages() const {
  return {"[OPTIONS]"};
}

std::string NodeCreateFlagParserDelegate::Description() const {
  return "Create nodes";
}

std::vector<NamedHelpType> NodeCreateFlagParserDelegate::CollectNamedHelps()
    const {
  return {
      std::make_pair(YELLOW_COLORED("Options:"),
                     std::vector<std::string>{
                         name_flag_->help(),
                         port_flag_->help(),
                     }),
  };
}

}  // namespace felicia