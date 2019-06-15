#ifndef FELICIA_CORE_MESSAGE_MESSAGE_IO_H_
#define FELICIA_CORE_MESSAGE_MESSAGE_IO_H_

#include "google/protobuf/message.h"
#include "third_party/chromium/net/base/io_buffer.h"

#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/header.h"

namespace felicia {

enum MessageIoError {
#define MESSAGE_IO_ERR(ERR, _) ERR,
#include "felicia/core/message/message_io_error_list.h"
#undef MESSAGE_IO_ERR
};

std::string MessageIoErrorToString(MessageIoError mesasge_io_error);

template <typename T, typename SFINAE = void>
class MessageIO;

template <typename T>
class MessageIO<T, std::enable_if_t<
                       std::is_base_of<::google::protobuf::Message, T>::value ||
                       std::is_same<DynamicProtobufMessage, T>::value>> {
 public:
  static MessageIoError SerializeToString(const T* proto, std::string* text) {
    if (!proto->SerializeToString(text))
      return MessageIoError::ERR_FAILED_TO_SERIALIZE;

    return MessageIoError::OK;
  }

  static MessageIoError ParseHeaderFromBuffer(char* buffer, Header* header) {
    if (!Header::FromBytes(buffer, header))
      return MessageIoError::ERR_CORRUPTED_HEADER;

    return MessageIoError::OK;
  }

  static MessageIoError ParseMessageFromBuffer(char* buffer,
                                               const Header& header,
                                               bool buffer_include_header,
                                               T* proto) {
    std::string text;
    char* start = buffer;
    if (buffer_include_header) {
      start += sizeof(Header);
    }
    if (!proto->ParseFromArray(start, header.size()))
      return MessageIoError::ERR_FAILED_TO_PARSE;

    return MessageIoError::OK;
  }

  static MessageIoError AttachToBuffer(
      const std::string& text, scoped_refptr<::net::GrowableIOBuffer> buffer,
      int* size) {
    // This should be before return `ERR_NOT_ENOUGH_BUFFER`. Caller might use
    // this |size| to reallocate buffer.
    *size = sizeof(Header) + text.length();

    if (buffer->RemainingCapacity() < *size)
      return MessageIoError::ERR_NOT_ENOUGH_BUFFER;

    Header header;
    header.set_size(text.length());
    memcpy(buffer->StartOfBuffer(), &header, sizeof(Header));
    memcpy(buffer->StartOfBuffer() + sizeof(Header), text.data(),
           text.length());
    return MessageIoError::OK;
  }
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_MESSAGE_IO_H_