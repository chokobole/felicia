#include "felicia/core/channel/udp_channel_base.h"

#include <utility>

#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

void UDPChannelBase::OnRead(int result) {
  if (result >= 0 || result == ::net::ERR_MSG_TOO_BIG) {
    std::move(read_callback_).Run(Status::OK());
  } else {
    std::move(read_callback_)
        .Run(errors::NetworkError(::net::ErrorToString(result)));
  }
}

}  // namespace felicia