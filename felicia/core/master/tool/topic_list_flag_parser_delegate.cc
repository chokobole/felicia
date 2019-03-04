#include "felicia/core/master/tool/topic_list_flag_parser_delegate.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

TopicListFlagParserDelegate::TopicListFlagParserDelegate() {
  {
    BoolFlag::Builder builder(MakeValueStore(&all_));
    auto flag = builder.SetShortName("-a")
                    .SetLongName("--all")
                    .SetHelp("List all the topics")
                    .Build();
    all_flag_ = std::make_unique<BoolFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&topic_));
    auto flag = builder.SetShortName("-t")
                    .SetLongName("--topic")
                    .SetHelp("List topics with a given topic")
                    .Build();
    topic_flag_ = std::make_unique<StringFlag>(flag);
  }
}

TopicListFlagParserDelegate::~TopicListFlagParserDelegate() = default;

bool TopicListFlagParserDelegate::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, all_flag_, topic_flag_);
}

bool TopicListFlagParserDelegate::Validate() const {
  int is_set_cnt = 0;
  if (all_flag_->is_set()) is_set_cnt++;
  if (topic_flag_->is_set()) is_set_cnt++;

  return is_set_cnt == 1;
}

std::vector<std::string> TopicListFlagParserDelegate::CollectUsages() const {
  return {"[OPTIONS]"};
}

std::string TopicListFlagParserDelegate::Description() const {
  return "List topics";
}

std::vector<NamedHelpType> TopicListFlagParserDelegate::CollectNamedHelps()
    const {
  return {
      std::make_pair(TextStyle::Yellow("Options:"),
                     std::vector<std::string>{
                         all_flag_->help(),
                         topic_flag_->help(),
                     }),
  };
}

}  // namespace felicia