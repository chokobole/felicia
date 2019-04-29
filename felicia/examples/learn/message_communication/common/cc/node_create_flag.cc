#include "felicia/examples/learn/message_communication/common/cc/node_create_flag.h"

#include <iostream>

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

NodeCreateFlag::NodeCreateFlag() {
  {
    BoolFlag::Builder builder(MakeValueStore(&is_publishing_node_));
    auto flag = builder.SetShortName("-p")
                    .SetHelp(
                        "create publishing node, if set, default: false(create "
                        "subscribing node)")
                    .Build();
    is_publishing_node_flag_ = std::make_unique<BoolFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&name_));
    auto flag = builder.SetShortName("-n")
                    .SetLongName("--name")
                    .SetHelp("name for node")
                    .Build();
    name_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&topic_));
    auto flag =
        builder.SetShortName("-t")
            .SetLongName("--topic")
            .SetHelp("topic to publish or subscribe, it's required to be set")
            .Build();
    topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringChoicesFlag::Builder builder(MakeValueStore<std::string>(
        &channel_type_, "TCP", Choices<std::string>{"TCP", "UDP"}));
    auto flag = builder.SetShortName("-c")
                    .SetLongName("--channel_type")
                    .SetHelp(
                        "protocol to deliver message, it only works for "
                        "publishing node")
                    .Build();
    channel_type_flag_ = std::make_unique<StringChoicesFlag>(flag);
  }
}

NodeCreateFlag::~NodeCreateFlag() = default;

bool NodeCreateFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, is_publishing_node_flag_, name_flag_,
                             topic_flag_, channel_type_flag_);
}

bool NodeCreateFlag::Validate() const { return CheckIfFlagWasSet(topic_flag_); }

}  // namespace felicia