#ifndef FELICIA_CORE_MASTER_TOOL_TOPIC_SUBSCRIBE_FLAG_H_
#define FELICIA_CORE_MASTER_TOOL_TOPIC_SUBSCRIBE_FLAG_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class TopicSubscribeFlag : public FlagParser::Delegate {
 public:
  TopicSubscribeFlag();
  ~TopicSubscribeFlag();

  const StringFlag* topic_flag() const { return topic_flag_.get(); }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  std::string topic_;
  std::unique_ptr<StringFlag> topic_flag_;

  DISALLOW_COPY_AND_ASSIGN(TopicSubscribeFlag);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_TOPIC_SUBSCRIBE_FLAG_H_