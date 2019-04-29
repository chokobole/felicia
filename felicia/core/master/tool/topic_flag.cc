#include "felicia/core/master/tool/topic_flag.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

static const char* kLs = "ls";
static const char* kPublish = "publish";
static const char* kSubscribe = "subscribe";

TopicFlag::TopicFlag() : current_command_(COMMAND_SELF) {
  {
    StringChoicesFlag::Builder builder(MakeValueStore<std::string>(
        &command_, ::base::EmptyString(),
        Choices<std::string>{kLs, kPublish, kSubscribe}));
    auto flag = builder.SetName("COMMAND").Build();
    command_flag_ = std::make_unique<StringChoicesFlag>(flag);
  }
}

TopicFlag::~TopicFlag() = default;

bool TopicFlag::Parse(FlagParser& parser) {
  switch (current_command_) {
    case COMMAND_SELF:
      if (command_flag_->Parse(parser)) {
        if (strings::Equals(command_, kLs)) {
          current_command_ = COMMAND_LIST;
        } else if (strings::Equals(command_, kPublish)) {
          current_command_ = COMMAND_PUBLISH;
        } else if (strings::Equals(command_, kSubscribe)) {
          current_command_ = COMMAND_SUBSCRIBE;
        }
        parser.set_program_name(::base::StringPrintf(
            "%s %s", parser.program_name().c_str(), command_.c_str()));
        return true;
      }
      return false;
    case COMMAND_LIST:
      return list_delegate_.Parse(parser);
    case COMMAND_PUBLISH:
      return publish_delegate_.Parse(parser);
    case COMMAND_SUBSCRIBE:
      return subscribe_delegate_.Parse(parser);
  }
}

bool TopicFlag::Validate() const {
  switch (current_command_) {
    case COMMAND_SELF:
      return false;
    case COMMAND_LIST:
      return list_delegate_.Validate();
    case COMMAND_PUBLISH:
      return publish_delegate_.Validate();
    case COMMAND_SUBSCRIBE:
      return subscribe_delegate_.Validate();
  }
}

std::vector<std::string> TopicFlag::CollectUsages() const {
  switch (current_command_) {
    case COMMAND_SELF:
      return {"COMMAND"};
    case COMMAND_LIST:
      return list_delegate_.CollectUsages();
    case COMMAND_PUBLISH:
      return publish_delegate_.CollectUsages();
    case COMMAND_SUBSCRIBE:
      return subscribe_delegate_.CollectUsages();
  }
}

std::string TopicFlag::Description() const {
  switch (current_command_) {
    case COMMAND_SELF:
      return "Manage topics";
    case COMMAND_LIST:
      return list_delegate_.Description();
    case COMMAND_PUBLISH:
      return publish_delegate_.Description();
    case COMMAND_SUBSCRIBE:
      return subscribe_delegate_.Description();
  }
}

std::vector<NamedHelpType> TopicFlag::CollectNamedHelps() const {
  switch (current_command_) {
    case COMMAND_SELF: {
      return {
          std::make_pair(
              kYellowCommands,
              std::vector<std::string>{
                  MakeNamedHelpText(kLs, list_delegate_.Description()),
                  MakeNamedHelpText(kPublish, publish_delegate_.Description()),
                  MakeNamedHelpText(kSubscribe,
                                    subscribe_delegate_.Description()),
              }),
      };
    }
    case COMMAND_LIST:
      return list_delegate_.CollectNamedHelps();
    case COMMAND_PUBLISH:
      return publish_delegate_.CollectNamedHelps();
    case COMMAND_SUBSCRIBE:
      return subscribe_delegate_.CollectNamedHelps();
  }
}

}  // namespace felicia