#if defined(HAS_ROS)

#include "felicia/core/channel/ros_service_response.h"

namespace felicia {

RosServiceResponse::RosServiceResponse(std::unique_ptr<Channel> channel)
    : channel_(std::move(channel)) {}

RosServiceResponse::~RosServiceResponse() = default;

void RosServiceResponse::OnReceiveRequest(
    const RosServiceResponseHeader& header, const Status& s) {
  channel_->SetDynamicReceiveBuffer(false);
  Status new_status = s;
  if (new_status.ok()) {
    RosServiceRequestHeader expected;
    expected.md5sum = header.md5sum;
    new_status = receiver_.message().Validate(expected);
  }

  MessageSender<RosServiceResponseHeader> sender(channel_.get());
  channel_->SetDynamicSendBuffer(true);
  if (!new_status.ok()) {
    LOG(ERROR) << new_status;
    RosServiceResponseHeader errored_header;
    errored_header.error = new_status.error_message();
    sender.SendMessage(errored_header,
                       base::BindOnce(&RosServiceResponse::OnResponse,
                                      base::Unretained(this), true));
  } else {
    sender.SendMessage(header, base::BindOnce(&RosServiceResponse::OnResponse,
                                              base::Unretained(this), false));
  }
}

void RosServiceResponse::OnResponse(bool sent_error, const Status& s) {
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