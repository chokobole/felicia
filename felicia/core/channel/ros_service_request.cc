#if defined(HAS_ROS)

#include "felicia/core/channel/ros_service_request.h"

namespace felicia {

RosServiceRequest::RosServiceRequest() = default;

RosServiceRequest::~RosServiceRequest() = default;

void RosServiceRequest::Reset() {
  receiver_.Reset();
  callback_.Reset();
}

void RosServiceRequest::OnRequest(const Status& s) {
  LOG_IF(ERROR, !s.ok()) << s;
  channel_->SetDynamicSendBuffer(false);
}

void RosServiceRequest::OnReceiveResponse(const RosServiceRequestHeader& header,
                                          const Status& s) {
  channel_->SetDynamicReceiveBuffer(false);
  Status new_status = s;
  if (new_status.ok() && receiver_.message().error.empty()) {
    RosServiceResponseHeader expected;
    expected.md5sum = header.md5sum;
    new_status = receiver_.message().Validate(expected);
  }

  std::move(callback_).Run(new_status);
}

}  // namespace felicia

#endif  // defined(HAS_ROS)