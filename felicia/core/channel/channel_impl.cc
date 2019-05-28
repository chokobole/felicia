#include "felicia/core/channel/channel_impl.h"

#include "third_party/chromium/base/logging.h"

namespace felicia {

ChannelImpl::~ChannelImpl() = default;

bool ChannelImpl::IsSocket() const { return false; }

Socket* ChannelImpl::ToSocket() {
  DCHECK(IsSocket());
  return reinterpret_cast<Socket*>(this);
}

}  // namespace felicia