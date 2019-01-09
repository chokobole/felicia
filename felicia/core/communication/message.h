#ifndef FELICIA_CORE_COMMUNICATION_MESSAGE_H_
#define FELICIA_CORE_COMMUNICATION_MESSAGE_H_

#include <string>

#include "google/protobuf/message.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/logging.h"

namespace felicia {

class EXPORT Message {
 public:
  enum MessageType {
    MESSAGE_TYPE_UNINTIALIZED,
    MESSAGE_TYPE_PROTOBUF,
  };

  Message();
  explicit Message(const google::protobuf::Message& protobuf_message);
  Message(const Message& message);
  ~Message();
  void operator=(const Message& message);

  void reset();

  google::protobuf::Message* protobuf_message() const {
    DCHECK(IsProtobufMessage());
    return value_.protobuf_message_;
  }

  void set_protobuf_message(const google::protobuf::Message& protobuf_message);

  void Clear();
  std::string ToSerealizedString() const;
  Message ParseFromString(const std::string& data) const;

  bool IsUninitialized() const { return type_ == MESSAGE_TYPE_UNINTIALIZED; }
  bool IsProtobufMessage() const { return type_ == MESSAGE_TYPE_PROTOBUF; }

 private:
  union MessageValue {
    google::protobuf::Message* protobuf_message_;

    MessageValue() = default;
    MessageValue(google::protobuf::Message* protobuf_message)
        : protobuf_message_(protobuf_message) {}
  };

  MessageType type_;
  MessageValue value_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_MESSAGE_H_