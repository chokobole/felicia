#include "felicia/core/master/tool/topic_publish_flag.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

TopicPublishFlag::TopicPublishFlag() {
  {
    StringFlag::Builder builder(MakeValueStore(&content_));
    auto flag =
        builder.SetName("content").SetHelp("Content to publish").Build();
    content_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&topic_));
    auto flag = builder.SetShortName("-t")
                    .SetLongName("--topic")
                    .SetHelp("Topic to publish")
                    .Build();
    topic_flag_ = std::make_unique<StringFlag>(flag);
  }
}

TopicPublishFlag::~TopicPublishFlag() = default;

bool TopicPublishFlag::Parse(FlagParser& parser) {
  PARSE_POSITIONAL_FLAG(parser, 1, content_flag_);
  return PARSE_OPTIONAL_FLAG(parser, topic_flag_);
}

bool TopicPublishFlag::Validate() const {
  return content_flag_->is_set() && topic_flag_->is_set();
}

std::vector<std::string> TopicPublishFlag::CollectUsages() const {
  return {"[OPTIONS]"};
}

std::string TopicPublishFlag::Description() const { return "Publish topics"; }

std::vector<NamedHelpType> TopicPublishFlag::CollectNamedHelps() const {
  return {
      std::make_pair(TextStyle::Yellow("Options:"),
                     std::vector<std::string>{
                         content_flag_->help(),
                         topic_flag_->help(),
                     }),
  };
}

}  // namespace felicia