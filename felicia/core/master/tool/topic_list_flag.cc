#include "felicia/core/master/tool/topic_list_flag.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

TopicListFlag::TopicListFlag() {
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
                    .SetHelp("List a given topic")
                    .Build();
    topic_flag_ = std::make_unique<StringFlag>(flag);
  }
}

TopicListFlag::~TopicListFlag() = default;

bool TopicListFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, all_flag_, topic_flag_);
}

bool TopicListFlag::Validate() const {
  return CheckIfOneOfFlagWasSet(all_flag_, topic_flag_);
}

std::vector<std::string> TopicListFlag::CollectUsages() const {
  return {"[OPTIONS]"};
}

std::string TopicListFlag::Description() const { return "List topics"; }

std::vector<NamedHelpType> TopicListFlag::CollectNamedHelps() const {
  return {
      std::make_pair(kYellowOptions,
                     std::vector<std::string>{
                         all_flag_->help(),
                         topic_flag_->help(),
                     }),
  };
}

}  // namespace felicia