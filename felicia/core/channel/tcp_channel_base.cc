#include "felicia/core/channel/tcp_channel_base.h"

namespace felicia {

TCPChannelBase::TCPChannelBase() = default;
TCPChannelBase::~TCPChannelBase() = default;

TCPClientChannel* TCPChannelBase::ToTCPClientChannel() {
  DCHECK(IsClient());
  return reinterpret_cast<TCPClientChannel*>(this);
}

TCPServerChannel* TCPChannelBase::ToTCPServerChannel() {
  DCHECK(IsServer());
  return reinterpret_cast<TCPServerChannel*>(this);
}

}  // namespace felicia