#ifndef FELICIA_CORE_MASTER_TOOL_TOPIC_FLAG_H_
#define FELICIA_CORE_MASTER_TOOL_TOPIC_FLAG_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/core/master/tool/topic_list_flag.h"
#include "felicia/core/master/tool/topic_publish_flag.h"
#include "felicia/core/master/tool/topic_subscribe_flag.h"
#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class TopicFlag : public FlagParser::Delegate {
 public:
  enum Command {
    COMMAND_SELF,
    COMMAND_LIST,
    COMMAND_PUBLISH,
    COMMAND_SUBSCRIBE,
  };

  TopicFlag();
  ~TopicFlag();

  const TopicListFlag& list_delegate() const { return list_delegate_; }
  const TopicPublishFlag& publish_delegate() const { return publish_delegate_; }
  const TopicSubscribeFlag& subscribe_delegate() const {
    return subscribe_delegate_;
  }
  Command command() const { return current_command_; }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  std::string command_;
  std::unique_ptr<StringChoicesFlag> command_flag_;
  TopicListFlag list_delegate_;
  TopicPublishFlag publish_delegate_;
  TopicSubscribeFlag subscribe_delegate_;
  Command current_command_;

  DISALLOW_COPY_AND_ASSIGN(TopicFlag);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_TOPIC_FLAG_H_