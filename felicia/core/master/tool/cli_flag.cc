#include "felicia/core/master/tool/cli_flag.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

static constexpr const char* kClient = "client";
static constexpr const char* kNode = "node";
static constexpr const char* kTopic = "topic";

CliFlag::CliFlag() : current_command_(COMMAND_SELF) {
  {
    StringChoicesFlag::Builder builder(MakeValueStore<std::string>(
        &command_, base::EmptyString(),
        Choices<std::string>{kClient, kNode, kTopic}));
    auto flag = builder.SetName("COMMAND").Build();
    command_flag_ = std::make_unique<StringChoicesFlag>(flag);
  }
}

CliFlag::~CliFlag() = default;

bool CliFlag::Parse(FlagParser& parser) {
  switch (current_command_) {
    case COMMAND_SELF:
      if (command_flag_->Parse(parser)) {
        if (Equals(command_, kClient)) {
          current_command_ = COMMAND_CLIENT;
        } else if (Equals(command_, kNode)) {
          current_command_ = COMMAND_NODE;
        } else if (Equals(command_, kTopic)) {
          current_command_ = COMMAND_TOPIC;
        }
        parser.set_program_name(base::StringPrintf(
            "%s %s", parser.program_name().c_str(), command_.c_str()));
        return true;
      }
      return false;
    case COMMAND_CLIENT:
      return client_delegate_.Parse(parser);
    case COMMAND_NODE:
      return node_delegate_.Parse(parser);
    case COMMAND_TOPIC:
      return topic_delegate_.Parse(parser);
  }
}

bool CliFlag::Validate() const {
  switch (current_command_) {
    case COMMAND_SELF:
      return false;
    case COMMAND_CLIENT:
      return client_delegate_.Validate();
    case COMMAND_NODE:
      return node_delegate_.Validate();
    case COMMAND_TOPIC:
      return topic_delegate_.Validate();
  }
}

std::vector<std::string> CliFlag::CollectUsages() const {
  switch (current_command_) {
    case COMMAND_SELF:
      return {"[OPTIONS]", "COMMAND"};
    case COMMAND_CLIENT:
      return client_delegate_.CollectUsages();
    case COMMAND_NODE:
      return node_delegate_.CollectUsages();
    case COMMAND_TOPIC:
      return topic_delegate_.CollectUsages();
  }
}

std::string CliFlag::Description() const {
  switch (current_command_) {
    case COMMAND_SELF:
      return "A proxy command to manage felicia";
    case COMMAND_CLIENT:
      return client_delegate_.Description();
    case COMMAND_NODE:
      return node_delegate_.Description();
    case COMMAND_TOPIC:
      return topic_delegate_.Description();
  }
}

std::vector<NamedHelpType> CliFlag::CollectNamedHelps() const {
  switch (current_command_) {
    case COMMAND_SELF: {
      return {
          std::make_pair(TextStyle::Yellow("Management commands:"),
                         std::vector<std::string>{
                             MakeNamedHelpText(kClient, "Manage clients"),
                             MakeNamedHelpText(kNode, "Manage nodes"),
                             MakeNamedHelpText(kTopic, "Manage topics"),
                         }),
      };
    }
    case COMMAND_CLIENT:
      return client_delegate_.CollectNamedHelps();
    case COMMAND_NODE:
      return node_delegate_.CollectNamedHelps();
    case COMMAND_TOPIC:
      return topic_delegate_.CollectNamedHelps();
  }
}

}  // namespace felicia