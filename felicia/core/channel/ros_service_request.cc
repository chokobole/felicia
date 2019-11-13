// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#include "felicia/core/channel/ros_service_request.h"

namespace felicia {

RosServiceRequest::RosServiceRequest() = default;

RosServiceRequest::~RosServiceRequest() = default;

void RosServiceRequest::Reset() {
  receiver_.Reset();
  callback_.Reset();
}

void RosServiceRequest::OnRequest(Status s) {
  LOG_IF(ERROR, !s.ok()) << s;
  channel_->SetDynamicSendBuffer(false);
}

void RosServiceRequest::OnReceiveResponse(const RosServiceRequestHeader& header,
                                          Status s) {
  channel_->SetDynamicReceiveBuffer(false);
  if (s.ok() && receiver_.message().error.empty()) {
    RosServiceResponseHeader expected;
    expected.md5sum = header.md5sum;
    s = receiver_.message().Validate(expected);
  }

  std::move(callback_).Run(std::move(s));
}

}  // namespace felicia

#endif  // defined(HAS_ROS)