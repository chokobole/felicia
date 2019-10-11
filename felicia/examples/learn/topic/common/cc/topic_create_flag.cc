#include "felicia/examples/learn/topic/common/cc/topic_create_flag.h"

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

TopicCreateFlag::TopicCreateFlag() {
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
    BoolFlag::Builder builder(MakeValueStore(&use_ssl_));
    auto flag =
        builder.SetLongName("--use_ssl")
            .SetHelp("use communication via ssl connection, default: false")
            .Build();
    use_ssl_flag_ = std::make_unique<BoolFlag>(flag);
  }

  {
    StringFlag::Builder builder(MakeValueStore(&topic_));
    auto flag = builder.SetShortName("-t")
                    .SetLongName("--topic")
                    .SetHelp("name for topic, it's required to be set")
                    .Build();
    topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringChoicesFlag::Builder builder(MakeValueStore<std::string>(
        &channel_type_, ChannelDef_Type_Name(ChannelDef::CHANNEL_TYPE_TCP),
        Choices<std::string>{
            ChannelDef_Type_Name(ChannelDef::CHANNEL_TYPE_TCP),
            ChannelDef_Type_Name(ChannelDef::CHANNEL_TYPE_UDP),
            ChannelDef_Type_Name(ChannelDef::CHANNEL_TYPE_UDS),
            ChannelDef_Type_Name(ChannelDef::CHANNEL_TYPE_SHM),
        }));
    auto flag =
        builder.SetShortName("-c")
            .SetLongName("--channel_type")
            .SetHelp(base::StringPrintf(
                "protocol to deliver message, it only works for "
                "publishing node (default: %s)",
                ChannelDef_Type_Name(ChannelDef::CHANNEL_TYPE_TCP).c_str()))
            .Build();
    channel_type_flag_ = std::make_unique<StringChoicesFlag>(flag);
  }
}

TopicCreateFlag::~TopicCreateFlag() = default;

bool TopicCreateFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, is_publishing_node_flag_, use_ssl_flag_,
                             name_flag_, topic_flag_, channel_type_flag_);
}

bool TopicCreateFlag::Validate() const {
  return CheckIfFlagWasSet(topic_flag_);
}

}  // namespace felicia