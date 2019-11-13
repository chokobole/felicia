// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#include "felicia/core/message/ros_rpc_header.h"

namespace felicia {

RosRpcHeader::RosRpcHeader() = default;

RosRpcHeader::~RosRpcHeader() = default;

MessageIOError RosRpcHeader::AttachHeader(const std::string& content,
                                          std::string* text) {
  size_ = content.length();
  text->resize(header_size() + size_);
  char* buffer = const_cast<char*>(text->c_str());
  memcpy(buffer, &ok_, sizeof(bool));
  buffer += sizeof(bool);
  memcpy(buffer, &size_, sizeof(int));
  buffer += sizeof(int);
  memcpy(buffer, content.c_str(), size_);
  return MessageIOError::OK;
}

int RosRpcHeader::header_size() const { return sizeof(bool) + sizeof(int); }

MessageIOError RosRpcHeader::ParseHeader(const char* buffer,
                                         int* mesasge_offset,
                                         int* message_size) {
  ok_ = *reinterpret_cast<const bool*>(buffer);
  buffer += sizeof(bool);
  size_ = *reinterpret_cast<const int*>(buffer);
  *message_size = size_;
  *mesasge_offset = header_size();
  return MessageIOError::OK;
}

bool RosRpcHeader::ok() const { return ok_; }

void RosRpcHeader::set_ok(bool ok) { ok_ = ok; }

}  // namespace felicia

#endif  // defined(HAS_ROS)