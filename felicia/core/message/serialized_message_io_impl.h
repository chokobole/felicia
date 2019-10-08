#ifndef FELICIA_CORE_MESSAGE_SERIALIZED_MESSAGE_IO_IMPL_H_
#define FELICIA_CORE_MESSAGE_SERIALIZED_MESSAGE_IO_IMPL_H_

#include "felicia/core/message/serialized_message.h"

#include "third_party/chromium/base/strings/string_util.h"

namespace felicia {

template <typename T>
class MessageIOImpl<
    T, std::enable_if_t<std::is_same<T, SerializedMessage>::value>> {
 public:
  static MessageIOError Serialize(const T* serialized_msg, std::string* text) {
    *text = serialized_msg->serialized();
    return MessageIOError::OK;
  }

  static MessageIOError Deserialize(const char* start, size_t size,
                                    T* serialized_msg) {
    serialized_msg->set_serialized(std::string(start, size));
    return MessageIOError::OK;
  }

  static std::string TypeName() { return base::EmptyString(); }

  static std::string Definition() { return base::EmptyString(); }

  static std::string MD5Sum() { return base::EmptyString(); }
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_SERIALIZED_MESSAGE_IO_IMPL_H_