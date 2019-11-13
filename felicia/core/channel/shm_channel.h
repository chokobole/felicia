#ifndef FELICIA_CORE_CHANNEL_SHM_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_SHM_CHANNEL_H_

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/settings.h"
#include "felicia/core/channel/shared_memory/platform_handle_broker.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class FEL_EXPORT ShmChannel : public Channel {
 public:
  ~ShmChannel() override;

  bool IsShmChannel() const override;

  ChannelDef::Type type() const override;

  bool ShouldReceiveMessageWithHeader() const override;

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override;

  void SetSendBufferSize(Bytes bytes) override;
  void SetReceiveBufferSize(Bytes bytes) override;

  StatusOr<ChannelDef> MakeSharedMemory();

 private:
  friend class ChannelFactory;

  explicit ShmChannel(
      const channel::ShmSettings& settings = channel::ShmSettings());

  void OnReceiveData(StatusOr<PlatformHandleBroker::Data> status_or);
  void FillData(PlatformHandleBroker::Data* handle_info);

  bool TrySetEnoughReceiveBufferSize(int capacity) override;

  channel::ShmSettings settings_;
  PlatformHandleBroker broker_;
  StatusOnceCallback connect_callback_;

  DISALLOW_COPY_AND_ASSIGN(ShmChannel);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SHM_CHANNEL_H_