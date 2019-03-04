#include "felicia/core/master/client/client.h"

#include <algorithm>
#include <limits>

#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/no_destructor.h"
#include "third_party/chromium/base/rand_util.h"

#include "felicia/core/util/uuid/generator.h"

namespace felicia {

namespace {

struct RandUint32Traits {
  static uint32_t Generate() {
    return ::base::RandGenerator(std::numeric_limits<uint32_t>::max());
  };
};

Generator<uint32_t, RandUint32Traits>& GetIDGenerator() {
  static ::base::NoDestructor<Generator<uint32_t, RandUint32Traits>>
      id_generator;
  return *id_generator;
}

}  // namespace

// static
std::unique_ptr<Client> Client::NewClient(const ClientInfo& client_info) {
  ClientInfo new_client_info;
  new_client_info.CopyFrom(client_info);
  new_client_info.set_id(GetIDGenerator().Generate());
  return ::base::WrapUnique(new Client(new_client_info));
}

Client::~Client() { GetIDGenerator().Return(client_info().id()); }

void Client::AddNode(std::unique_ptr<Node> node) {
  DFAKE_SCOPED_LOCK(add_remove_);
  nodes_.push_back(std::move(node));
}

void Client::RemoveNode(const NodeInfo& node_info) {
  DFAKE_SCOPED_LOCK(add_remove_);
  nodes_.erase(
      std::remove_if(nodes_.begin(), nodes_.end(), NodeNameChecker{node_info}),
      nodes_.end());
}

bool Client::HasNode(const NodeInfo& node_info) const {
  DFAKE_SCOPED_LOCK(add_remove_);
  return std::find_if(nodes_.begin(), nodes_.end(),
                      NodeNameChecker{node_info}) != nodes_.end();
}

::base::WeakPtr<Node> Client::FindNode(const NodeInfo& node_info) {
  DFAKE_SCOPED_LOCK(add_remove_);
  auto it =
      std::find_if(nodes_.begin(), nodes_.end(), NodeNameChecker{node_info});
  if (it == nodes_.end()) {
    return nullptr;
  }

  return (*it)->AsWeakPtr();
}

std::vector<::base::WeakPtr<Node>> Client::FindNodes(
    const NodeFilter& node_filter) {
  DFAKE_SCOPED_LOCK(add_remove_);
  std::vector<::base::WeakPtr<Node>> nodes;
  if (node_filter.all()) {
    for (auto& node : nodes_) {
      nodes.push_back(node->AsWeakPtr());
    }
  } else if (!node_filter.publishing_topic().empty()) {
    for (auto& node : nodes_) {
      if (node->IsPublishingTopic(node_filter.publishing_topic())) {
        nodes.push_back(node->AsWeakPtr());
#if !DCHECK_IS_ON()
        break;
#endif
      }
    }
    // Publishing node should be only one.
    DCHECK(nodes.size() == 0 || nodes.size() == 1);
  } else if (!node_filter.subscribing_topic().empty()) {
    for (auto& node : nodes_) {
      if (node->IsSubsribingTopic(node_filter.subscribing_topic())) {
        nodes.push_back(node->AsWeakPtr());
      }
    }
  }

  return nodes;
}

}  // namespace felicia