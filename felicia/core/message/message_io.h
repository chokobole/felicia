#ifndef FELICIA_CORE_MESSAGE_MESSAGE_IO_H_
#define FELICIA_CORE_MESSAGE_MESSAGE_IO_H_

#include "third_party/chromium/net/base/io_buffer.h"

#include "felicia/core/message/header.h"

namespace felicia {

enum MessageIOError {
#define MESSAGE_IO_ERR(ERR, _) ERR,
#include "felicia/core/message/message_io_error_list.h"
#undef MESSAGE_IO_ERR
};

std::string MessageIOErrorToString(MessageIOError mesasge_io_error);

template <typename T, typename SFINAE = void>
class MessageIOImpl;

class MessageIO {
 public:
  template <typename T>
  static MessageIOError SerializeToString(const T* message, std::string* text) {
    return MessageIOImpl<T>::Serialize(message, text);
  }

  static MessageIOError ParseHeaderFromBuffer(const char* buffer,
                                              Header* header);

  template <typename T>
  static MessageIOError ParseMessageFromBuffer(const char* buffer,
                                               const Header& header,
                                               bool buffer_include_header,
                                               T* message) {
    std::string text;
    const char* start = buffer;
    if (buffer_include_header) {
      start += sizeof(Header);
    }
    return MessageIOImpl<T>::Deserialize(start, header.size(), message);
  }

  static MessageIOError AttachToBuffer(
      const std::string& text, scoped_refptr<net::GrowableIOBuffer> buffer,
      int* size) {
    // This should be before return `ERR_NOT_ENOUGH_BUFFER`. Caller might use
    // this |size| to reallocate buffer.
    *size = sizeof(Header) + text.length();

    if (buffer->RemainingCapacity() < *size)
      return MessageIOError::ERR_NOT_ENOUGH_BUFFER;

    Header header;
    header.set_size(text.length());
    memcpy(buffer->StartOfBuffer(), &header, sizeof(Header));
    memcpy(buffer->StartOfBuffer() + sizeof(Header), text.data(),
           text.length());
    return MessageIOError::OK;
  }
};

}  // namespace felicia

#include "felicia/core/message/protobuf_message_io_impl.h"
#include "felicia/core/message/ros_message_io_impl.h"

#endif  // FELICIA_CORE_MESSAGE_MESSAGE_IO_H_