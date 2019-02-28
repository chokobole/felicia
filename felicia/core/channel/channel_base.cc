#include "felicia/core/channel/channel_base.h"

#include <utility>

#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

void ChannelBase::OnRead(int result) {
  if (result >= 0) {
    std::move(read_callback_).Run(Status::OK());
  } else {
    std::move(read_callback_)
        .Run(errors::NetworkError(::net::ErrorToString(result)));
  }
}

void ChannelBase::OnWrite(int result) {
  if (result >= 0) {
    std::move(write_callback_).Run(Status::OK());
  } else {
    std::move(write_callback_)
        .Run(errors::NetworkError(::net::ErrorToString(result)));
  }
}

// static
const int32_t ChannelBase::kMaxReceiverBufferSize = 5 * 1000 * 1000;

}  // namespace felicia