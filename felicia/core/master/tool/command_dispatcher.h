// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MASTER_TOOL_COMMAND_DISPATCHER_H_
#define FELICIA_CORE_MASTER_TOOL_COMMAND_DISPATCHER_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/core/communication/dynamic_publisher.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/tool/cli_flag.h"
#include "felicia/core/master/tool/topic_publish_command_dispatcher.h"
#include "felicia/core/master/tool/topic_subscribe_command_dispatcher.h"
#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/protobuf/master.pb.h"

namespace felicia {

class CommandDispatcher {
 public:
  CommandDispatcher();

  void Dispatch(const CliFlag& delegate) const;

 private:
  // Client Commands
  void Dispatch(const ClientFlag& delegate) const;
  void Dispatch(const ClientListFlag& delegate) const;

  void OnListClientsAsync(const ListClientsRequest* request,
                          ListClientsResponse* response, Status s) const;

  // Node Commands
  void Dispatch(const NodeFlag& delegate) const;
  void Dispatch(const NodeListFlag& delegate) const;

  void OnListNodesAsync(const ListNodesRequest* request,
                        ListNodesResponse* response, Status s) const;

  // Service Commands
  void Dispatch(const ServiceFlag& delegate) const;
  void Dispatch(const ServiceListFlag& delegate) const;

  void OnListServicesAsync(const ListServicesRequest* request,
                           ListServicesResponse* response, Status s) const;

  // Topic Commands
  void Dispatch(const TopicFlag& delegate) const;
  void Dispatch(const TopicListFlag& delegate) const;
  void Dispatch(const TopicPublishFlag& delegate) const;
  void Dispatch(const TopicSubscribeFlag& delegate) const;

  void OnListTopicsAsync(const ListTopicsRequest* request,
                         ListTopicsResponse* response, Status s) const;

  TopicPublishCommandDispatcher topic_publish_command_dispatcher_;
  TopicSubscribeCommandDispatcher topic_subscribe_command_dispatcher_;

  DISALLOW_COPY_AND_ASSIGN(CommandDispatcher);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_COMMAND_DISPATCHER_H_