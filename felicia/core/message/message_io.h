#ifndef FELICIA_CORE_MESSAGE_MESSAGE_IO_H_
#define FELICIA_CORE_MESSAGE_MESSAGE_IO_H_

#include "google/protobuf/message.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/websockets/websocket_frame.h"

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
  static MessageIoError SerializeToBuffer(
      const T* proto, scoped_refptr<::net::GrowableIOBuffer> buffer,
      bool via_websocket, int* size) {
    std::string text;
    if (!proto->SerializeToString(&text))
      return MessageIoError::ERR_FAILED_TO_SERIALIZE;

    return AttachToBuffer(text, buffer, via_websocket, size);
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

 private:
  static MessageIoError AttachToBuffer(
      const std::string& text, scoped_refptr<::net::GrowableIOBuffer> buffer,
      bool via_websocket, int* size) {
    // This should be before return `ERR_NOT_ENOUGH_BUFFER`. Caller might use
    // this |size| to reallocate buffer.
    if (via_websocket) {
      ::net::WebSocketFrame frame(::net::WebSocketFrameHeader::kOpCodeBinary);
      ::net::WebSocketFrameHeader& header = frame.header;
      header.final = true;
      header.masked = false;
      header.payload_length = text.length();
      *size = ::net::GetWebSocketFrameHeaderSize(frame.header) +
              frame.header.payload_length;

      if (buffer->RemainingCapacity() < *size)
        return MessageIoError::ERR_NOT_ENOUGH_BUFFER;

      int header_size = ::net::WriteWebSocketFrameHeader(
          frame.header, nullptr, buffer->StartOfBuffer(), *size);
      memcpy(buffer->StartOfBuffer() + header_size, text.data(), text.length());
    } else {
      *size = sizeof(Header) + text.length();

      if (buffer->RemainingCapacity() < *size)
        return MessageIoError::ERR_NOT_ENOUGH_BUFFER;

      Header header;
      header.set_size(text.length());
      memcpy(buffer->StartOfBuffer(), &header, sizeof(Header));
      memcpy(buffer->StartOfBuffer() + sizeof(Header), text.data(),
             text.length());
    }

    return MessageIoError::OK;
  }
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_MESSAGE_IO_H_