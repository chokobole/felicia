#ifndef FELICIA_CORE_MASTER_TOOL_TOPIC_SUBSCRIBE_COMMAND_DISPATCHER_H_
#define FELICIA_CORE_MASTER_TOOL_TOPIC_SUBSCRIBE_COMMAND_DISPATCHER_H_

#include "felicia/core/master/tool/cli_flag.h"
#include "felicia/core/message/protobuf_loader.h"

namespace felicia {

class TopicSubscribeCommandDispatcher {
 public:
  TopicSubscribeCommandDispatcher();

  void Dispatch(ProtobufLoader* protobuf_loader,
                const TopicSubscribeFlag& delegate) const;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_TOPIC_SUBSCRIBE_COMMAND_DISPATCHER_H_