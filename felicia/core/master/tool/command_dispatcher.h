#ifndef FELICIA_CORE_MASTER_TOOL_COMMAND_DISPATCHER_H_
#define FELICIA_CORE_MASTER_TOOL_COMMAND_DISPATCHER_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/core/communication/dynamic_publisher.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/tool/cli_flag.h"
#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/protobuf_loader.h"
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

  void OnListClientsAsync(ListClientsRequest* request,
                          ListClientsResponse* response, const Status& s) const;

  // Node Commands
  void Dispatch(const NodeFlag& delegate) const;
  void Dispatch(const NodeListFlag& delegate) const;

  void OnListNodesAsync(ListNodesRequest* request, ListNodesResponse* response,
                        const Status& s) const;

  // Topic Commands
  void Dispatch(const TopicFlag& delegate) const;
  void Dispatch(const TopicListFlag& delegate) const;
  void Dispatch(const TopicPublishFlag& delegate) const;
  void Dispatch(const TopicSubscribeFlag& delegate) const;

  void OnListTopicsAsync(ListTopicsRequest* request,
                         ListTopicsResponse* response, const Status& s) const;

  void PublishMessageFromJSON(const std::string& message, int64_t delay,
                              DynamicPublisher* publisher) const;

  void OnPublish(const Status& s) const;

  void OnNewMessage(const std::string& topic,
                    const DynamicProtobufMessage& message) const;

  void OnSubscriptionError(const std::string& topic, const Status& s) const;

  mutable std::unique_ptr<ProtobufLoader> protobuf_loader_;

  DISALLOW_COPY_AND_ASSIGN(CommandDispatcher);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_COMMAND_DISPATCHER_H_