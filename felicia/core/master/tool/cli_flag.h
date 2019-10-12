#ifndef FELICIA_CORE_MASTER_TOOL_CLI_FLAG_H_
#define FELICIA_CORE_MASTER_TOOL_CLI_FLAG_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/core/master/tool/client_flag.h"
#include "felicia/core/master/tool/node_flag.h"
#include "felicia/core/master/tool/service_flag.h"
#include "felicia/core/master/tool/topic_flag.h"
#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class CliFlag : public FlagParser::Delegate {
 public:
  enum Command {
    COMMAND_SELF,
    COMMAND_CLIENT,
    COMMAND_NODE,
    COMMAND_SERVICE,
    COMMAND_TOPIC,
  };

  CliFlag();
  ~CliFlag();

  const ClientFlag& client_delegate() const { return client_delegate_; }
  const NodeFlag& node_delegate() const { return node_delegate_; }
  const ServiceFlag& service_delegate() const { return service_delegate_; }
  const TopicFlag& topic_delegate() const { return topic_delegate_; }
  Command command() const { return current_command_; }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  std::string command_;
  std::unique_ptr<StringChoicesFlag> command_flag_;
  ClientFlag client_delegate_;
  NodeFlag node_delegate_;
  ServiceFlag service_delegate_;
  TopicFlag topic_delegate_;
  Command current_command_;

  DISALLOW_COPY_AND_ASSIGN(CliFlag);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_CLI_FLAG_H_