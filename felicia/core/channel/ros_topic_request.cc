#if defined(HAS_ROS)

#include "felicia/core/channel/ros_topic_request.h"

namespace felicia {

RosTopicRequest::RosTopicRequest() = default;

RosTopicRequest::~RosTopicRequest() = default;

void RosTopicRequest::Reset() {
  receiver_.Reset();
  callback_.Reset();
}

void RosTopicRequest::OnRequest(Status s) {
  LOG_IF(ERROR, !s.ok()) << s;
  channel_->SetDynamicSendBuffer(false);
}

void RosTopicRequest::OnReceiveResponse(const RosTopicRequestHeader& header,
                                        Status s) {
  channel_->SetDynamicReceiveBuffer(false);
  if (s.ok() && receiver_.message().error.empty()) {
    RosTopicResponseHeader expected;
    expected.md5sum = header.md5sum;
    expected.topic = header.topic;
    expected.type = header.type;
    s = receiver_.message().Validate(expected);
  }

  std::move(callback_).Run(std::move(s));
}

}  // namespace felicia

#endif  // defined(HAS_ROS)