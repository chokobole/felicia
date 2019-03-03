#ifndef FELICIA_CORE_MESSAGE_MESSAGE_H_
#define FELICIA_CORE_MESSAGE_MESSAGE_H_

#include "google/protobuf/message.h"
#include "third_party/chromium/base/memory/ref_counted.h"
#include "third_party/chromium/net/base/io_buffer.h"

#include "felicia/core/message/header.h"

namespace felicia {

class MessageBase {
 public:
  MessageBase() = default;

  const Header& header() const { return header_; }

 protected:
  // Header always should be set in serializer.
  void set_heaer(const Header& header) { header_ = header; }

  Header header_;
};

template <typename T, typename SFINAE = void>
class Message;

template <typename T>
class Message<
    T, std::enable_if_t<std::is_base_of<::google::protobuf::Message, T>::value>>
    : public MessageBase {
 public:
  static bool SerializeToBuffer(
      const T* proto, scoped_refptr<::net::IOBufferWithSize>* buffer) {
    std::string text;
    bool ret = proto->SerializeToString(&text);
    if (!ret) return false;

    Header header;
    header.set_size(text.length());
    *buffer = ::base::MakeRefCounted<::net::IOBufferWithSize>(sizeof(Header) +
                                                              text.length());
    memcpy((*buffer)->data(), &header, sizeof(Header));
    memcpy((*buffer)->data() + sizeof(Header), text.data(), text.length());

    return true;
  }

  static bool ParseFromBuffer(scoped_refptr<::net::IOBufferWithSize> buffer,
                              T* proto) {
    Header header;
    if (!Header::FromBytes(buffer->data(), &header)) return false;

    std::string text(buffer->data() + sizeof(Header), header.size());
    if (!proto->ParseFromString(text)) return false;

    return true;
  }
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_MESSAGE_H_