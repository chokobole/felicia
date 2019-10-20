#ifndef FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_

#include "felicia/core/channel/channel.h"

namespace felicia {

class EXPORT UDPChannel : public Channel {
 public:
  ~UDPChannel() override;

  bool IsUDPChannel() const override;

  ChannelDef::Type type() const override;

  bool ShouldReceiveMessageWithHeader() const override;

  StatusOr<ChannelDef> Bind();

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override;

  void SetSendBufferSize(Bytes bytes) override;
  void SetReceiveBufferSize(Bytes bytes) override;

 private:
  friend class ChannelFactory;

  UDPChannel();

  bool TrySetEnoughReceiveBufferSize(int capacity) override;

  DISALLOW_COPY_AND_ASSIGN(UDPChannel);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_