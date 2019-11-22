// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MESSAGE_ROS_PROTOCOL_H_
#define FELICIA_CORE_MESSAGE_ROS_PROTOCOL_H_

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/strings/str_util.h"

namespace felicia {

static constexpr const char* kRosProtocol = "ros://";
static constexpr const char* kRosRpcProtocol = "rosrpc://";

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

inline bool IsUsingRosProtocol(const std::string& text) {
  return StartsWith(text, kRosProtocol);
}

// If |text| starts with |kRosRpcProtocol|, return true and |out| will be the
// substring after |kRosRpcProtocol|. Otherwise return false and no changes into
// |out|.
inline bool ConsumeRosRpcProtocol(const std::string& text, std::string* out) {
  base::StringPiece t = text;
  bool ret = ConsumePrefix(&t, kRosRpcProtocol);
  if (ret) {
    *out = t.as_string();
  }
  return ret;
}

inline std::string AttachRosRpcProtocol(const std::string& host,
                                        uint16_t port) {
  return base::StringPrintf("%s%s:%u", kRosRpcProtocol, host.c_str(), port);
}

inline bool IsUsingRosRpcProtocol(const std::string& text) {
  return StartsWith(text, kRosRpcProtocol);
}

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_ROS_PROTOCOL_H_