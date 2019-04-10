#ifndef FELICIA_CORE_MESSAGE_MESSAGE_IO_H_
#define FELICIA_CORE_MESSAGE_MESSAGE_IO_H_

#include "google/protobuf/message.h"
#include "third_party/chromium/net/base/io_buffer.h"

#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/header.h"

namespace felicia {

template <typename T, typename SFINAE = void>
class MessageIO;

template <typename T>
class MessageIO<T, std::enable_if_t<
                       std::is_base_of<::google::protobuf::Message, T>::value ||
                       std::is_same<DynamicProtobufMessage, T>::value>> {
 public:
  static bool SerializeToBuffer(const T* proto, ::net::IOBuffer* buffer,
                                size_t* size) {
    std::string text;
    if (!proto->SerializeToString(&text)) return false;

    Header header;
    header.set_size(text.length());
    memcpy(buffer->data(), &header, sizeof(Header));
    memcpy(buffer->data() + sizeof(Header), text.data(), text.length());

    *size = sizeof(Header) + text.length();

    return true;
  }

  static bool ParseHeaderFromBuffer(::net::IOBuffer* buffer, Header* header) {
    if (!Header::FromBytes(buffer->data(), header)) return false;

    return true;
  }

  static bool ParseMessageFromBuffer(::net::IOBuffer* buffer,
                                     const Header& header,
                                     bool buffer_include_header, T* proto) {
    std::string text;
    char* start = buffer->data();
    if (buffer_include_header) {
      start += sizeof(Header);
    }
    if (!proto->ParseFromArray(start, header.size())) return false;

    return true;
  }
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_MESSAGE_IO_H_