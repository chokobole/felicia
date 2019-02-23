#ifndef FELICIA_CORE_MASTER_TOOL_COMMAND_DISPATCHER_H_
#define FELICIA_CORE_MASTER_TOOL_COMMAND_DISPATCHER_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/master/client/grpc_master_client.h"
#include "felicia/core/master/tool/flag_parser_delegate.h"
#include "felicia/core/master/tool/node_create_flag_parser_delegate.h"
#include "felicia/core/master/tool/node_flag_parser_delegate.h"
#include "felicia/core/master/tool/node_get_flag_parser_delegate.h"

namespace felicia {

class CommandDispatcher {
 public:
  explicit CommandDispatcher(std::unique_ptr<GrpcMasterClient> client);

  void Dispatch(const FlagParserDelegate& delegate) const;

 private:
  void Dispatch(const NodeFlagParserDelegate& delegate) const;
  void Dispatch(const NodeCreateFlagParserDelegate& delegate) const;
  void Dispatch(const NodeGetFlagParserDelegate& delegate) const;

  std::unique_ptr<GrpcMasterClient> master_client_;

  DISALLOW_COPY_AND_ASSIGN(CommandDispatcher);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_COMMAND_DISPATCHER_H_