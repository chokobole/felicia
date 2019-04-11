#ifndef FELICIA_CORE_MASTER_TOOL_NODE_LIST_FLAG_H_
#define FELICIA_CORE_MASTER_TOOL_NODE_LIST_FLAG_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class NodeListFlag : public FlagParser::Delegate {
 public:
  NodeListFlag();
  ~NodeListFlag();

  const BoolFlag* all_flag() const { return all_flag_.get(); }
  const StringFlag* publishing_topic_flag() const {
    return publishing_topic_flag_.get();
  }
  const StringFlag* subscribing_topic_flag() const {
    return subscribing_topic_flag_.get();
  }
  const StringFlag* name_flag() const { return name_flag_.get(); }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  bool all_;
  std::string publishing_topic_;
  std::string subscribing_topic_;
  std::string name_;
  std::unique_ptr<BoolFlag> all_flag_;
  std::unique_ptr<StringFlag> publishing_topic_flag_;
  std::unique_ptr<StringFlag> subscribing_topic_flag_;
  std::unique_ptr<StringFlag> name_flag_;

  DISALLOW_COPY_AND_ASSIGN(NodeListFlag);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_NODE_LIST_FLAG_H_