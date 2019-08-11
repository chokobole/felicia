#include "felicia/core/node/node.h"

#include <algorithm>

#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/no_destructor.h"
#include "third_party/chromium/base/rand_util.h"
#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/util/uuid/generator.h"
#include "third_party/chromium/base/logging.h"

namespace felicia {

namespace {

constexpr size_t kNameLength = 12;

struct RandNameTraits {
  static std::string Generate() {
    const char* alphadigit =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t len = strlen(alphadigit);

    std::string text;
    text.resize(kNameLength);

    for (size_t i = 0; i < kNameLength; ++i) {
      text[i] = alphadigit[base::RandGenerator(len)];
    }

    return text;
  };

  static std::string InvalidValue() { return base::EmptyString(); }
};

Generator<std::string, RandNameTraits>& GetNameGenerator() {
  static base::NoDestructor<Generator<std::string, RandNameTraits>>
      name_generator;
  return *name_generator;
}

}  // namespace

std::unique_ptr<Node> Node::NewNode(const NodeInfo& node_info) {
  if (!node_info.name().empty()) {
    if (GetNameGenerator().In(node_info.name())) {
      return nullptr;
    } else {
      GetNameGenerator().Add(node_info.name());
      return base::WrapUnique(new Node(node_info));
    }
  }

  std::string name = GetNameGenerator().Generate();
  if (name.length() == 0) {
    LOG(ERROR) << "Failed to generate name for node";
    return nullptr;
  }

  NodeInfo new_node_info;
  new_node_info.CopyFrom(node_info);
  new_node_info.set_name(name);
  return base::WrapUnique(new Node(new_node_info));
}

Node::Node(const NodeInfo& node_info) : node_info_(node_info) {}

Node::~Node() { GetNameGenerator().Return(node_info_.name()); }

void Node::RegisterPublishingTopic(const TopicInfo& topic_info) {
  topic_info_map_[topic_info.topic()] = topic_info;
}

void Node::RegisterSubscribingTopic(const std::string& topic) {
  subscribing_topics_.emplace_back(topic);
}

void Node::UnregisterPublishingTopic(const std::string& topic) {
  auto it = topic_info_map_.find(topic);
  topic_info_map_.erase(it);
}

void Node::UnregisterSubscribingTopic(const std::string& topic) {
  auto it = std::remove_if(subscribing_topics_.begin(),
                           subscribing_topics_.end(), StringComparator{topic});
  subscribing_topics_.erase(it, subscribing_topics_.end());
}

bool Node::IsPublishingTopic(const std::string& topic) const {
  auto it = topic_info_map_.find(topic);
  return it != topic_info_map_.end();
}

bool Node::IsSubsribingTopic(const std::string& topic) const {
  auto it = std::find_if(subscribing_topics_.begin(), subscribing_topics_.end(),
                         StringComparator{topic});
  return it != subscribing_topics_.end();
}

const TopicInfo& Node::GetTopicInfo(const std::string& topic) const {
  DCHECK(IsPublishingTopic(topic));
  return topic_info_map_.find(topic)->second;
}

std::vector<TopicInfo> Node::AllPublishingTopicInfos() const {
  std::vector<TopicInfo> topic_infos;
  auto it = topic_info_map_.begin();
  while (it != topic_info_map_.end()) {
    topic_infos.push_back(it->second);
    it++;
  }
  return topic_infos;
}

std::vector<std::string> Node::AllSubscribingTopics() const {
  std::vector<std::string> topics;
  auto it = subscribing_topics_.begin();
  while (it != subscribing_topics_.end()) {
    topics.push_back(*it);
    it++;
  }
  return topics;
}

bool NodeNameChecker::operator()(const std::unique_ptr<Node>& node) {
  return node->name() == node_info_.name();
}

}  // namespace felicia