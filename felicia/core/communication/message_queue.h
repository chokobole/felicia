#ifndef FELICIA_CORE_COMMUNICATION_MESSAGE_QUEUE_H_
#define FELICIA_CORE_COMMUNICATION_MESSAGE_QUEUE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "felicia/core/communication/message.h"
#include "felicia/core/lib/base/macros.h"
#include "felicia/core/lib/containers/fast_pool.h"

namespace felicia {

using MessagePool = FastPool<Message, uint8_t>;

class EXPORT MessageQueue {
 public:
  MessageQueue();

  // Allocate |MesasgePool| with size.  Before controlloing any queue,
  // (such as Enque, Pop) allocate queue first with size you expecte
  // to be enough.
  void AllocateQueue(const std::string& topic_name, uint8_t size);
  void EnqueMessage(const std::string& topic_name, const Message& message);
  Message& PopMessage(const std::string& topic_name);
  bool HaveMessage(const std::string& topic_name);

 private:
  std::unordered_map<std::string, MessagePool> message_map_;

  DISALLOW_COPY_AND_ASSIGN(MessageQueue);
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_MESSAGE_QUEUE_H_
