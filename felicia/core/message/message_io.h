#ifndef FELICIA_CORE_MESSAGE_MESSAGE_IO_H_
#define FELICIA_CORE_MESSAGE_MESSAGE_IO_H_

#include "google/protobuf/message.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/websockets/websocket_deflater.h"
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

template <typename T>
class DeflateTraits {
 public:
  static bool ShouldDeflate(const T* proto, const std::string& serialized) {
    return serialized.length() > 1024;
  }
};

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

  static MessageIoError AttachToBuffer(
      const std::string& text, scoped_refptr<::net::GrowableIOBuffer> buffer,
      ::net::WebSocketDeflater* deflater, int* size) {
    // This should be before return `ERR_NOT_ENOUGH_BUFFER`. Caller might use
    // this |size| to reallocate buffer.
    ::net::WebSocketFrame frame(::net::WebSocketFrameHeader::kOpCodeBinary);
    ::net::WebSocketFrameHeader& header = frame.header;
    header.final = true;
    header.masked = false;
    scoped_refptr<::net::IOBufferWithSize> compressed_payload;
    char* payload = const_cast<char*>(text.data());
    header.payload_length = text.length();
    if (deflater) {
      header.reserved1 = true;

      if (!deflater->AddBytes(payload,
                              static_cast<size_t>(header.payload_length))) {
        DVLOG(1) << "WebSocket protocol error. "
                 << "deflater->AddBytes() returns an error.";
        return MessageIoError::ERR_WS_PROTOCOL_ERROR;
      }
      if (header.final && !deflater->Finish()) {
        DVLOG(1) << "WebSocket protocol error. "
                 << "deflater->Finish() returns an error.";
        return MessageIoError::ERR_WS_PROTOCOL_ERROR;
      }

      compressed_payload = deflater->GetOutput(deflater->CurrentOutputSize());
      if (!compressed_payload.get()) {
        DVLOG(1) << "WebSocket protocol error. "
                 << "deflater->GetOutput() returns an error.";
        return MessageIoError::ERR_WS_PROTOCOL_ERROR;
      }

      payload = compressed_payload->data();
      header.payload_length = compressed_payload->size();
    }

    *size = ::net::GetWebSocketFrameHeaderSize(header) +
            frame.header.payload_length;

    if (buffer->RemainingCapacity() < *size)
      return MessageIoError::ERR_NOT_ENOUGH_BUFFER;

    int header_size = ::net::WriteWebSocketFrameHeader(
        frame.header, nullptr, buffer->StartOfBuffer(), *size);
    memcpy(buffer->StartOfBuffer() + header_size, payload,
           header.payload_length);

    return MessageIoError::OK;
  }
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_MESSAGE_IO_H_