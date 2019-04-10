#ifndef FELICIA_CORE_MESSAGE_DYNAMIC_PROTOBUF_MESSAGE_H_
#define FELICIA_CORE_MESSAGE_DYNAMIC_PROTOBUF_MESSAGE_H_

#include "google/protobuf/message.h"

namespace felicia {

class DynamicProtobufMessage {
 public:
  DynamicProtobufMessage();
  DynamicProtobufMessage(const DynamicProtobufMessage& other);
  DynamicProtobufMessage& operator=(const DynamicProtobufMessage& other);
  ~DynamicProtobufMessage();

  void Reset(::google::protobuf::Message* message);

  std::string DebugString() const;

  bool SerializeToString(std::string* text) const;
  bool ParseFromArray(const char* data, size_t size);

 private:
  ::google::protobuf::Message* message_ = nullptr;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_DYNAMIC_PROTOBUF_MESSAGE_H_