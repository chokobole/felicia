#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

NodeLifecycle::NodeLifecycle() = default;

NodeLifecycle::~NodeLifecycle() = default;

void NodeLifecycle::OnInit() {}

void NodeLifecycle::OnDidCreate(const NodeInfo& node_info) {}

void NodeLifecycle::OnError(Status status) { LOG(ERROR) << status; }

}  // namespace felicia