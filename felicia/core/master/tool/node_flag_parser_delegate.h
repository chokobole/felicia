#ifndef FELICIA_CORE_MASTER_TOOL_NODE_PARSER_DELEGATE_H_
#define FELICIA_CORE_MASTER_TOOL_NODE_PARSER_DELEGATE_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/master/tool/node_create_flag_parser_delegate.h"
#include "felicia/core/master/tool/node_get_flag_parser_delegate.h"
#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class EXPORT NodeFlagParserDelegate : public FlagParser::Delegate {
 public:
  enum Command {
    COMMAND_SELF,
    COMMAND_CREATE,
    COMMAND_GET,
  };

  NodeFlagParserDelegate();
  ~NodeFlagParserDelegate();

  const NodeCreateFlagParserDelegate& create_delegate() const {
    return create_delegate_;
  }
  const NodeGetFlagParserDelegate& get_delegate() const {
    return get_delegate_;
  }
  Command command() const { return current_command_; }

  bool Parse(FlagParser& parser) override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  std::string command_;
  std::unique_ptr<StringChoicesFlag> command_flag_;
  NodeCreateFlagParserDelegate create_delegate_;
  NodeGetFlagParserDelegate get_delegate_;
  Command current_command_;

  DISALLOW_COPY_AND_ASSIGN(NodeFlagParserDelegate);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_NODE_PARSER_DELEGATE_H_