#ifndef FELICIA_CORE_MASTER_TOOL_NODE_GET_PARSER_DELEGATE_H_
#define FELICIA_CORE_MASTER_TOOL_NODE_GET_PARSER_DELEGATE_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class EXPORT NodeGetFlagParserDelegate : public FlagParser::Delegate {
 public:
  NodeGetFlagParserDelegate();
  ~NodeGetFlagParserDelegate();

  bool all() const { return all_; }
  ::base::StringPiece publishing_topic() const { return publishing_topic_; }
  ::base::StringPiece subscribing_topic() const { return subscribing_topic_; }

  bool Parse(FlagParser& parser) override;

  bool Validate() override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  bool all_;
  std::string publishing_topic_;
  std::string subscribing_topic_;
  std::unique_ptr<BoolFlag> all_flag_;
  std::unique_ptr<StringFlag> publishing_topic_flag_;
  std::unique_ptr<StringFlag> subscribing_topic_flag_;

  DISALLOW_COPY_AND_ASSIGN(NodeGetFlagParserDelegate);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_NODE_GET_PARSER_DELEGATE_H_