#include "felicia/core/master/tool/topic_publish_flag.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

TopicPublishFlag::TopicPublishFlag() {
  {
    StringFlag::Builder builder(MakeValueStore(&topic_));
    auto flag = builder.SetName("topic").SetHelp("Topic to publish").Build();
    topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&type_));
    auto flag = builder.SetName("type").SetHelp("Type of message").Build();
    type_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&message_));
    auto flag = builder.SetName("message")
                    .SetHelp("Content of message, in JSON format")
                    .Build();
    message_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    DefaultFlag<int64_t>::Builder builder(
        MakeValueStore<int64_t>(&interval_, 1000));
    auto flag =
        builder.SetShortName("-i")
            .SetLongName("--interval")
            .SetHelp(
                "Interval between messages, in milliseconds, default: 1000")
            .Build();
    interval_flag_ = std::make_unique<DefaultFlag<int64_t>>(flag);
  }
}

TopicPublishFlag::~TopicPublishFlag() = default;

bool TopicPublishFlag::Parse(FlagParser& parser) {
  PARSE_POSITIONAL_FLAG(parser, 3, topic_flag_, type_flag_, message_flag_);
  return PARSE_OPTIONAL_FLAG(parser, interval_flag_);
}

bool TopicPublishFlag::Validate() const {
  return CheckIfFlagWasSet(topic_flag_) && CheckIfFlagWasSet(type_flag_) &&
         CheckIfFlagWasSet(message_flag_);
}

std::vector<std::string> TopicPublishFlag::CollectUsages() const {
  return {"TOPIC TYPE MESSAGE [OPTIONS]"};
}

std::string TopicPublishFlag::Description() const { return "Publish topics"; }

std::vector<NamedHelpType> TopicPublishFlag::CollectNamedHelps() const {
  return {
      std::make_pair(TextStyle::Blue("Positions:"),
                     std::vector<std::string>{
                         topic_flag_->help(),
                         type_flag_->help(),
                         message_flag_->help(),
                     }),
      std::make_pair(TextStyle::Yellow("Options:"),
                     std::vector<std::string>{
                         interval_flag_->help(),
                     }),
  };
}

}  // namespace felicia