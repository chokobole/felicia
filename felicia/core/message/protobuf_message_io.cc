#include "felicia/core/message/message_io.h"

namespace felicia {

namespace protobuf_internal {

bool SerializeToString(const google::protobuf::Message* msg,
                       std::string* text) {
  return msg->SerializeToString(text);
}

bool ParseFromArray(google::protobuf::Message* msg, const char* start,
                    size_t size) {
  return msg->ParseFromArray(start, size);
}

}  // namespace protobuf_internal

}  // namespace felicia