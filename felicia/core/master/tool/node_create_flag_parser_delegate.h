#ifndef FELICIA_CORE_MASTER_TOOL_NODE_CREATE_PARSER_DELEGATE_H_
#define FELICIA_CORE_MASTER_TOOL_NODE_CREATE_PARSER_DELEGATE_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class NodeCreateFlagParserDelegate : public FlagParser::Delegate {
 public:
  NodeCreateFlagParserDelegate();
  ~NodeCreateFlagParserDelegate();

  ::base::StringPiece name() const { return name_; }
  int port() const { return port_; }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  std::string name_;
  int port_;
  std::unique_ptr<StringFlag> name_flag_;
  std::unique_ptr<IntFlag> port_flag_;

  DISALLOW_COPY_AND_ASSIGN(NodeCreateFlagParserDelegate);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_NODE_CREATE_PARSER_DELEGATE_H_