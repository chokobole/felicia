#if defined(HAS_ROS)

#include "felicia/core/channel/ros_topic_request.h"

namespace felicia {

RosTopicRequest::RosTopicRequest() = default;

RosTopicRequest::~RosTopicRequest() = default;

void RosTopicRequest::Reset() {
  receiver_.Reset();
  callback_.Reset();
}

void RosTopicRequest::OnRequest(const Status& s) {
  LOG_IF(ERROR, !s.ok()) << s;
  channel_->SetDynamicSendBuffer(false);
}

void RosTopicRequest::OnReceiveResponse(const RosTopicRequestHeader& header,
                                        const Status& s) {
  channel_->SetDynamicReceiveBuffer(false);
  Status new_status = s;
  if (new_status.ok() && receiver_.message().error.empty()) {
    RosTopicResponseHeader expected;
    expected.md5sum = header.md5sum;
    expected.topic = header.topic;
    expected.type = header.type;
    new_status = receiver_.message().Validate(expected);
  }

  std::move(callback_).Run(new_status);
}

}  // namespace felicia

#endif  // defined(HAS_ROS)