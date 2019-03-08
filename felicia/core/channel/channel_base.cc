#include "felicia/core/channel/channel_base.h"

#include <utility>

#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

// static
void ChannelBase::CallbackWithStatus(StatusCallback callback, int result) {
  if (result >= 0) {
    std::move(callback).Run(Status::OK());
  } else {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(result)));
  }
}

// static
const int32_t ChannelBase::kMaxReceiverBufferSize = 5 * 1000 * 1000;

}  // namespace felicia