#ifndef FELICIA_CORE_CHANNEL_UDP_CHANNEL_BASE_H_
#define FELICIA_CORE_CHANNEL_UDP_CHANNEL_BASE_H_

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/macros.h"

#include "felicia/core/channel/channel_base.h"

namespace felicia {

class UDPClientChannel;
class UDPServerChannel;

class EXPORT UDPChannelBase : public ChannelBase {
 public:
  UDPChannelBase();
  ~UDPChannelBase();

  UDPClientChannel* ToUDPClientChannel();
  UDPServerChannel* ToUDPServerChannel();

  void OnRead(int result);
  void OnWrite(int result);

  DISALLOW_COPY_AND_ASSIGN(UDPChannelBase);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_UDP_CHANNEL_BASE_H_