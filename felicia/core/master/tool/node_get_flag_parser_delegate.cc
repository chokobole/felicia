#include "felicia/core/master/tool/node_get_flag_parser_delegate.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

NodeGetFlagParserDelegate::NodeGetFlagParserDelegate() {
  {
    BoolFlag::Builder builder(MakeValueStore(&all_));
    auto flag = builder.SetShortName("-a")
                    .SetLongName("--all")
                    .SetHelp("Get all the nodes")
                    .Build();
    all_flag_ = std::make_unique<BoolFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&publishing_topic_));
    auto flag = builder.SetShortName("-p")
                    .SetLongName("--publishing_topic")
                    .SetHelp("Get nodes publishing a given topic")
                    .Build();
    publishing_topic_flag_ = std::make_unique<StringFlag>(flag);
  }

  {
    StringFlag::Builder builder(
        MakeValueStore<std::string>(&subscribing_topic_));
    auto flag = builder.SetShortName("-s")
                    .SetLongName("--subscribing_topic")
                    .SetHelp("Get nodes subscribing a given topic")
                    .Build();
    subscribing_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
}

NodeGetFlagParserDelegate::~NodeGetFlagParserDelegate() = default;

bool NodeGetFlagParserDelegate::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, all_flag_, publishing_topic_flag_,
                             subscribing_topic_flag_);
}

bool NodeGetFlagParserDelegate::Validate() {
  return all_flag_->is_set() || publishing_topic_flag_->is_set() ||
         subscribing_topic_flag_->is_set();
}

std::vector<std::string> NodeGetFlagParserDelegate::CollectUsages() const {
  return {"[OPTIONS]"};
}

std::string NodeGetFlagParserDelegate::Description() const {
  return "Get nodes";
}

std::vector<NamedHelpType> NodeGetFlagParserDelegate::CollectNamedHelps()
    const {
  return {
      std::make_pair(YELLOW_COLORED("Options:"),
                     std::vector<std::string>{
                         all_flag_->help(),
                         publishing_topic_flag_->help(),
                         subscribing_topic_flag_->help(),
                     }),
  };
}

}  // namespace felicia