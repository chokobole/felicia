#include "felicia/core/channel/udp_channel_base.h"

namespace felicia {

void UDPChannelBase::OnWrite(int result) {
  CallbackWithStatus(std::move(write_callback_), result);
}

void UDPChannelBase::OnRead(int result) {
  CallbackWithStatus(std::move(read_callback_), result);
}

}  // namespace felicia