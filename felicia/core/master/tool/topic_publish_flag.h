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

  const StringFlag* topic_flag() const { return topic_flag_.get(); }
  const StringFlag* message_type_flag() const {
    return message_type_flag_.get();
  }
  const StringFlag* message_flag() const { return message_flag_.get(); }
  const StringChoicesFlag* channel_type_flag() const {
    return channel_type_flag_.get();
  }
  const DefaultFlag<int64_t>* interval_flag() const {
    return interval_flag_.get();
  }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  std::string topic_;
  std::string message_type_;
  std::string message_;
  std::string channel_type_;
  int64_t interval_;
  std::unique_ptr<StringFlag> topic_flag_;
  std::unique_ptr<StringFlag> message_type_flag_;
  std::unique_ptr<StringFlag> message_flag_;
  std::unique_ptr<StringChoicesFlag> channel_type_flag_;
  std::unique_ptr<DefaultFlag<int64_t>> interval_flag_;

  DISALLOW_COPY_AND_ASSIGN(TopicPublishFlag);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_TOPIC_PUBLISH_FLAG_H_