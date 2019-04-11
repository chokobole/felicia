#ifndef FELICIA_CORE_MASTER_TOOL_TOPIC_PUBLISH_FLAG_H_
#define FELICIA_CORE_MASTER_TOOL_TOPIC_PUBLISH_FLAG_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class TopicPublishFlag : public FlagParser::Delegate {
 public:
  TopicPublishFlag();
  ~TopicPublishFlag();

  const StringFlag* content_flag() const { return content_flag_.get(); }
  const StringFlag* topic_flag() const { return topic_flag_.get(); }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  std::string content_;
  std::string topic_;
  std::unique_ptr<StringFlag> content_flag_;
  std::unique_ptr<StringFlag> topic_flag_;

  DISALLOW_COPY_AND_ASSIGN(TopicPublishFlag);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_TOPIC_PUBLISH_FLAG_H_