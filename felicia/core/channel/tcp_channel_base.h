#ifndef FELICIA_CORE_CHANNEL_TCP_CHANNEL_BASE_H_
#define FELICIA_CORE_CHANNEL_TCP_CHANNEL_BASE_H_

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/macros.h"

#include "felicia/core/channel/channel_base.h"

namespace felicia {

class TCPClientChannel;
class TCPServerChannel;

class TCPChannelBase : public ChannelBase {
 public:
  TCPChannelBase() = default;
  ~TCPChannelBase() override = default;

  TCPClientChannel* ToTCPClientChannel() {
    DCHECK(IsClient());
    return reinterpret_cast<TCPClientChannel*>(this);
  }

  TCPServerChannel* ToTCPServerChannel() {
    DCHECK(IsServer());
    return reinterpret_cast<TCPServerChannel*>(this);
  }

  DISALLOW_COPY_AND_ASSIGN(TCPChannelBase);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_TCP_CHANNEL_BASE_H_