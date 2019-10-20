#if defined(HAS_ROS)

#ifndef FELICIA_CORE_MESSAGE_ROS_HEADER_IO_H_
#define FELICIA_CORE_MESSAGE_ROS_HEADER_IO_H_

#include "felicia/core/message/ros_header.h"

#include "third_party/chromium/base/strings/string_util.h"

namespace felicia {

template <typename T>
class MessageIO<T, std::enable_if_t<std::is_base_of<RosHeader, T>::value>> {
 public:
  static MessageIOError Serialize(const T* ros_header, std::string* text) {
    ros_header->WriteToBuffer(text);
    return MessageIOError::OK;
  }

  static MessageIOError Deserialize(const char* start, size_t size,
                                    T* ros_header) {
    Status s = ros_header->ReadFromBuffer(start, size);
    if (!s.ok()) {
      return MessageIOError::ERR_FAILED_TO_PARSE;
    }
    return MessageIOError::OK;
  }

  static std::string TypeName() { return base::EmptyString(); }

  static std::string Definition() { return base::EmptyString(); }

  static std::string MD5Sum() { return base::EmptyString(); }
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_ROS_HEADER_IO_H_

#endif  // defined(HAS_ROS)