#include "felicia/examples/learn/message_communication/node_create_flag_parser_delegate.h"

namespace felicia {

NodeCreateFlagParserDelegate::NodeCreateFlagParserDelegate() {
  {
    StringFlag::Builder builder(MakeValueStore(&name_));
    auto flag = builder.SetLongName("--name").SetHelp("name").Build();
    name_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&topic_));
    auto flag = builder.SetLongName("--topic").SetHelp("topic").Build();
    topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringChoicesFlag::Builder builder(MakeValueStore<std::string>(
        &channel_type_, "TCP", Choices<std::string>{"TCP", "UDP"}));
    auto flag = builder.SetLongName("--channel_type").Build();
    channel_type_flag_ = std::make_unique<StringChoicesFlag>(flag);
  }
}

NodeCreateFlagParserDelegate::~NodeCreateFlagParserDelegate() = default;

bool NodeCreateFlagParserDelegate::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, name_flag_, topic_flag_,
                             channel_type_flag_);
}

bool NodeCreateFlagParserDelegate::Validate() const {
  return topic_flag_->is_set();
}

}  // namespace felicia