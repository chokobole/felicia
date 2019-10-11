#ifndef FELICIA_EXAMPLES_LEARN_TOPIC_COMMON_CC_TOPIC_CREATE_FLAG_H_
#define FELICIA_EXAMPLES_LEARN_TOPIC_COMMON_CC_TOPIC_CREATE_FLAG_H_

#include "felicia/examples/learn/common/cc/node_create_flag.h"

namespace felicia {

class TopicCreateFlag : public NodeCreateFlag {
 public:
  TopicCreateFlag();
  ~TopicCreateFlag();

  const BoolFlag* is_publishing_node_flag() const {
    return is_publishing_node_flag_.get();
  }
  const BoolFlag* use_ssl_flag() const { return use_ssl_flag_.get(); }
  const StringFlag* topic_flag() const { return topic_flag_.get(); }
  const StringChoicesFlag* channel_type_flag() const {
    return channel_type_flag_.get();
  }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(is_publishing_node_flag_,
                                          use_ssl_flag_, name_flag_,
                                          topic_flag_, channel_type_flag_)

 protected:
  bool is_publishing_node_;
  bool use_ssl_;
  std::string topic_;
  std::string channel_type_;
  std::unique_ptr<BoolFlag> is_publishing_node_flag_;
  std::unique_ptr<BoolFlag> use_ssl_flag_;
  std::unique_ptr<StringFlag> topic_flag_;
  std::unique_ptr<StringChoicesFlag> channel_type_flag_;

  DISALLOW_COPY_AND_ASSIGN(TopicCreateFlag);
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_TOPIC_COMMON_CC_TOPIC_CREATE_FLAG_H_