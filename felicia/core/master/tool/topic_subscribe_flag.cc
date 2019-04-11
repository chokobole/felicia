#include "felicia/core/master/tool/topic_subscribe_flag.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

TopicSubscribeFlag::TopicSubscribeFlag() {
  {
    StringFlag::Builder builder(MakeValueStore(&topic_));
    auto flag = builder.SetShortName("-t")
                    .SetLongName("--topic")
                    .SetHelp("Topic to subscribe")
                    .Build();
    topic_flag_ = std::make_unique<StringFlag>(flag);
  }
}

TopicSubscribeFlag::~TopicSubscribeFlag() = default;

bool TopicSubscribeFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, topic_flag_);
}

bool TopicSubscribeFlag::Validate() const { return topic_flag_->is_set(); }

std::vector<std::string> TopicSubscribeFlag::CollectUsages() const {
  return {"[OPTIONS]"};
}

std::string TopicSubscribeFlag::Description() const {
  return "Subscribe topics";
}

std::vector<NamedHelpType> TopicSubscribeFlag::CollectNamedHelps() const {
  return {
      std::make_pair(TextStyle::Yellow("Options:"),
                     std::vector<std::string>{
                         topic_flag_->help(),
                     }),
  };
}

}  // namespace felicia