#if defined(HAS_ROS)

#include "felicia/core/channel/ros_topic_response.h"

namespace felicia {

RosTopicResponse::RosTopicResponse(std::unique_ptr<Channel> channel)
    : channel_(std::move(channel)) {}

RosTopicResponse::~RosTopicResponse() = default;

void RosTopicResponse::OnReceiveRequest(const RosTopicResponseHeader& header,
                                        const Status& s) {
  channel_->SetDynamicReceiveBuffer(false);
  Status new_status = s;
  if (new_status.ok()) {
    RosTopicRequestHeader expected;
    expected.topic = header.topic;
    expected.md5sum = header.md5sum;
    expected.type = header.type;
    new_status = receiver_.message().Validate(expected);
  }

  MessageSender<RosTopicResponseHeader> sender(channel_.get());
  channel_->SetDynamicSendBuffer(true);
  if (!new_status.ok()) {
    LOG(ERROR) << new_status;
    RosTopicResponseHeader errored_header;
    errored_header.error = new_status.error_message();
    sender.SendMessage(errored_header,
                       base::BindOnce(&RosTopicResponse::OnResponse,
                                      base::Unretained(this), true));
  } else {
    sender.SendMessage(header, base::BindOnce(&RosTopicResponse::OnResponse,
                                              base::Unretained(this), false));
  }
}

void RosTopicResponse::OnResponse(bool sent_error, const Status& s) {
  if (s.ok()) {
    if (sent_error) return;

    channel_->SetDynamicSendBuffer(false);

    std::move(callback_).Run(std::move(channel_));
  } else {
    LOG(ERROR) << s;
  }

  delete this;
}

}  // namespace felicia

#endif  // defined(HAS_ROS)