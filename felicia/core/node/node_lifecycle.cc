// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

NodeLifecycle::NodeLifecycle() = default;

NodeLifecycle::~NodeLifecycle() = default;

void NodeLifecycle::OnInit() {}

void NodeLifecycle::OnDidCreate(NodeInfo node_info) {}

void NodeLifecycle::OnError(Status status) { LOG(ERROR) << status; }

}  // namespace felicia