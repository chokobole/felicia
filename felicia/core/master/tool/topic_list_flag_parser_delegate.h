#ifndef FELICIA_CORE_MASTER_TOOL_TOPIC_LIST_PARSER_DELEGATE_H_
#define FELICIA_CORE_MASTER_TOOL_TOPIC_LIST_PARSER_DELEGATE_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class TopicListFlagParserDelegate : public FlagParser::Delegate {
 public:
  TopicListFlagParserDelegate();
  ~TopicListFlagParserDelegate();

  bool all() const { return all_; }
  const std::string& topic() const { return topic_; }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  bool all_;
  std::string topic_;
  std::unique_ptr<BoolFlag> all_flag_;
  std::unique_ptr<StringFlag> topic_flag_;

  DISALLOW_COPY_AND_ASSIGN(TopicListFlagParserDelegate);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_TOPIC_LIST_PARSER_DELEGATE_H_