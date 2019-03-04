#include "felicia/core/master/tool/node_list_flag_parser_delegate.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

NodeListFlagParserDelegate::NodeListFlagParserDelegate() {
  {
    BoolFlag::Builder builder(MakeValueStore(&all_));
    auto flag = builder.SetShortName("-a")
                    .SetLongName("--all")
                    .SetHelp("List all the nodes")
                    .Build();
    all_flag_ = std::make_unique<BoolFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&publishing_topic_));
    auto flag = builder.SetShortName("-p")
                    .SetLongName("--publishing_topic")
                    .SetHelp("List nodes publishing a given topic")
                    .Build();
    publishing_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(
        MakeValueStore<std::string>(&subscribing_topic_));
    auto flag = builder.SetShortName("-s")
                    .SetLongName("--subscribing_topic")
                    .SetHelp("List nodes subscribing a given topic")
                    .Build();
    subscribing_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
}

NodeListFlagParserDelegate::~NodeListFlagParserDelegate() = default;

bool NodeListFlagParserDelegate::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, all_flag_, publishing_topic_flag_,
                             subscribing_topic_flag_);
}

bool NodeListFlagParserDelegate::Validate() const {
  return all_flag_->is_set() || publishing_topic_flag_->is_set() ||
         subscribing_topic_flag_->is_set();
}

std::vector<std::string> NodeListFlagParserDelegate::CollectUsages() const {
  return {"[OPTIONS]"};
}

std::string NodeListFlagParserDelegate::Description() const {
  return "List nodes";
}

std::vector<NamedHelpType> NodeListFlagParserDelegate::CollectNamedHelps()
    const {
  return {
      std::make_pair(TextStyle::Yellow("Options:"),
                     std::vector<std::string>{
                         all_flag_->help(),
                         publishing_topic_flag_->help(),
                         subscribing_topic_flag_->help(),
                     }),
  };
}

}  // namespace felicia