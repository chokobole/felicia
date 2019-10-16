#ifndef FELICIA_CORE_CHANNEL_ROS_PROTOCOL_H_
#define FELICIA_CORE_CHANNEL_ROS_PROTOCOL_H_

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/strings/str_util.h"

namespace felicia {

static constexpr const char* kRosProtocol = "ros://";

// If |text| starts with |kRosProtocols|, return true and |out| will be the
// substring after |kRosProtocols|. Otherwise return false and no changes into
// |out|.
inline bool ConsumeRosProtocol(const std::string& text, std::string* out) {
  base::StringPiece t = text;
  bool ret = ConsumePrefix(&t, kRosProtocol);
  if (ret) {
    *out = t.as_string();
  }
  return ret;
}

inline std::string AttachRosProtocol(const std::string& text) {
  return base::StringPrintf("%s%s", kRosProtocol, text.c_str());
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_ROS_PROTOCOL_H_