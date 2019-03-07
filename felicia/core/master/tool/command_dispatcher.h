#ifndef FELICIA_CORE_MASTER_TOOL_COMMAND_DISPATCHER_H_
#define FELICIA_CORE_MASTER_TOOL_COMMAND_DISPATCHER_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/master/rpc/grpc_master_client.h"
#include "felicia/core/master/tool/flag_parser_delegate.h"

namespace felicia {

class CommandDispatcher {
 public:
  explicit CommandDispatcher(std::unique_ptr<GrpcMasterClient> client);

  void Dispatch(const FlagParserDelegate& delegate) const;

 private:
  // Client Commands
  void Dispatch(const ClientFlagParserDelegate& delegate) const;
  void Dispatch(const ClientListFlagParserDelegate& delegate) const;

  void OnListClientsAsync(GrpcMasterClient* client, ListClientsRequest* request,
                          ListClientsResponse* response, const Status& s) const;

  // Node Commands
  void Dispatch(const NodeFlagParserDelegate& delegate) const;
  void Dispatch(const NodeListFlagParserDelegate& delegate) const;

  void OnListNodesAsync(GrpcMasterClient* client, ListNodesRequest* request,
                        ListNodesResponse* response, const Status& s) const;

  // Topic Commands
  void Dispatch(const TopicFlagParserDelegate& delegate) const;
  void Dispatch(const TopicListFlagParserDelegate& delegate) const;

  void OnListTopicsAsync(GrpcMasterClient* client, ListTopicsRequest* request,
                         ListTopicsResponse* response, const Status& s) const;

  std::unique_ptr<GrpcMasterClient> master_client_;

  DISALLOW_COPY_AND_ASSIGN(CommandDispatcher);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_COMMAND_DISPATCHER_H_