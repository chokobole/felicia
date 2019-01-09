#include "felicia/core/communication/message_queue.h"

#include "felicia/core/lib/base/logging.h"

namespace felicia {

MessageQueue::MessageQueue() = default;

void MessageQueue::AllocateQueue(const std::string& topic_name, uint8_t size) {
  MessagePool pool(size);
  message_map_[topic_name] = pool;
}

void MessageQueue::EnqueMessage(const std::string& topic_name,
                                const Message& message) {
  auto it = message_map_.find(topic_name);
  DCHECK(it != message_map_.end());
  message_map_[topic_name].push(message);
}

Message& MessageQueue::PopMessage(const std::string& topic_name) {
  auto it = message_map_.find(topic_name);
  DCHECK(it != message_map_.end());
  auto& messages = (*it).second;
  DCHECK(messages.length() > 0);
  auto& message = messages.front();
  messages.pop();
  return message;
}

bool MessageQueue::HaveMessage(const std::string& topic_name) {
  auto it = message_map_.find(topic_name);
  DCHECK(it != message_map_.end());
  auto& messages = (*it).second;
  return messages.length() > 0;
}

}  // namespace felicia
