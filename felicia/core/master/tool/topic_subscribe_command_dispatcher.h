// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MASTER_TOOL_TOPIC_SUBSCRIBE_COMMAND_DISPATCHER_H_
#define FELICIA_CORE_MASTER_TOOL_TOPIC_SUBSCRIBE_COMMAND_DISPATCHER_H_

#include "felicia/core/master/tool/cli_flag.h"

namespace felicia {

class TopicSubscribeCommandDispatcher {
 public:
  TopicSubscribeCommandDispatcher();

  void Dispatch(const TopicSubscribeFlag& delegate) const;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_TOPIC_SUBSCRIBE_COMMAND_DISPATCHER_H_