#include "felicia/core/channel/channel_base.h"

#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

void ChannelBase::OnRead(int result) {
  if (result >= 0) {
    read_callback_(Status::OK());
  } else {
    read_callback_(errors::NetworkError(::net::ErrorToString(result)));
  }
}

void ChannelBase::OnWrite(int result) {
  if (result >= 0) {
    write_callback_(Status::OK());
  } else {
    write_callback_(errors::NetworkError(::net::ErrorToString(result)));
  }
}

// static
const int32_t ChannelBase::kMaxReceiverBufferSize = 5 * 1000 * 1000;

}  // namespace felicia