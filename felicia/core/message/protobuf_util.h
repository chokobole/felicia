#ifndef FELICIA_CORE_MESSAGE_PROTOBUF_UTIL_H_
#define FELICIA_CORE_MESSAGE_PROTOBUF_UTIL_H_

#include "google/protobuf/message.h"

namespace felicia {
namespace protobuf {

std::string ToString(const ::google::protobuf::Message& message);

}  // namespace protobuf
}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_PROTOBUF_UTIL_H_