#if defined(HAS_ROS)

#include "felicia/core/channel/ros_service_response.h"

namespace felicia {

RosServiceResponse::RosServiceResponse(std::unique_ptr<Channel> channel)
    : channel_(std::move(channel)) {}

RosServiceResponse::~RosServiceResponse() = default;

void RosServiceResponse::OnReceiveRequest(
    const RosServiceResponseHeader& header, Status s) {
  channel_->SetDynamicReceiveBuffer(false);
  if (s.ok()) {
    RosServiceRequestHeader expected;
    expected.md5sum = header.md5sum;
    s = receiver_.message().Validate(expected);
  }

  MessageSender<RosServiceResponseHeader> sender(channel_.get());
  channel_->SetDynamicSendBuffer(true);
  if (!s.ok()) {
    LOG(ERROR) << s;
    RosServiceResponseHeader errored_header;
    errored_header.error = std::move(s).error_message();
    sender.SendMessage(errored_header,
                       base::BindOnce(&RosServiceResponse::OnResponse,
                                      base::Unretained(this), true));
  } else {
    sender.SendMessage(header, base::BindOnce(&RosServiceResponse::OnResponse,
                                              base::Unretained(this), false));
  }
}

void RosServiceResponse::OnResponse(bool sent_error, Status s) {
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