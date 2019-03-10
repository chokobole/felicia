#ifndef FELICIA_CORE_MASTER_TOOL_TOPIC_LIST_H_
#define FELICIA_CORE_MASTER_TOOL_TOPIC_LIST_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class TopicListFlag : public FlagParser::Delegate {
 public:
  TopicListFlag();
  ~TopicListFlag();

  const BoolFlag* all_flag() const { return all_flag_.get(); }
  const StringFlag* topic_flag() const { return topic_flag_.get(); }

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

  DISALLOW_COPY_AND_ASSIGN(TopicListFlag);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_TOPIC_LIST_H_