#include "felicia/core/channel/shared_memory/platform_handle_broker.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

PlatformHandleBroker::PlatformHandleBroker() = default;
PlatformHandleBroker::~PlatformHandleBroker() = default;

StatusOr<ChannelDef> PlatformHandleBroker::Setup(FillDataCallback callback) {
  return errors::Unimplemented("");
}

void PlatformHandleBroker::WaitForBroker(ChannelDef channel_def,
                                         ReceiveDataCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(receive_data_callback_.is_null());

  receive_data_callback_ = std::move(callback);
}

}  // namespace felicia