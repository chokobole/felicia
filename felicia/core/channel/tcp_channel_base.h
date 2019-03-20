#ifndef FELICIA_CORE_CHANNEL_TCP_CHANNEL_BASE_H_
#define FELICIA_CORE_CHANNEL_TCP_CHANNEL_BASE_H_

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/macros.h"

#include "felicia/core/channel/channel_base.h"

namespace felicia {

class TCPClientChannel;
class TCPServerChannel;

class EXPORT TCPChannelBase : public ChannelBase {
 public:
  TCPChannelBase();
  ~TCPChannelBase() override;

  TCPClientChannel* ToTCPClientChannel();
  TCPServerChannel* ToTCPServerChannel();

  virtual bool IsConnected() const = 0;

  DISALLOW_COPY_AND_ASSIGN(TCPChannelBase);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_TCP_CHANNEL_BASE_H_