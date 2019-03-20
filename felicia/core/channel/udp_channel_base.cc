#include "felicia/core/channel/udp_channel_base.h"

namespace felicia {

UDPChannelBase::UDPChannelBase() = default;
UDPChannelBase::~UDPChannelBase() = default;

UDPClientChannel* UDPChannelBase::ToUDPClientChannel() {
  DCHECK(IsClient());
  return reinterpret_cast<UDPClientChannel*>(this);
}

UDPServerChannel* UDPChannelBase::ToUDPServerChannel() {
  DCHECK(IsServer());
  return reinterpret_cast<UDPServerChannel*>(this);
}

void UDPChannelBase::OnWrite(int result) {
  CallbackWithStatus(std::move(write_callback_), result);
}

void UDPChannelBase::OnRead(int result) {
  CallbackWithStatus(std::move(read_callback_), result);
}

}  // namespace felicia