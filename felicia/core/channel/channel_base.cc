#include "felicia/core/channel/channel_base.h"

#include <utility>

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

ChannelBase::ChannelBase() = default;
ChannelBase::~ChannelBase() = default;

bool ChannelBase::IsClient() const { return false; }
bool ChannelBase::IsServer() const { return false; }

bool ChannelBase::IsTCPChannelBase() const { return false; }
bool ChannelBase::IsUDPChannelBase() const { return false; }

TCPChannelBase* ChannelBase::ToTCPChannelBase() {
  DCHECK(IsTCPChannelBase());
  return reinterpret_cast<TCPChannelBase*>(this);
}

UDPChannelBase* ChannelBase::ToUDPChannelBase() {
  DCHECK(IsUDPChannelBase());
  return reinterpret_cast<UDPChannelBase*>(this);
}

// static
void ChannelBase::CallbackWithStatus(StatusCallback callback, int result) {
  if (result >= 0) {
    std::move(callback).Run(Status::OK());
  } else {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(result)));
  }
}

// static
size_t ChannelBase::GetMaximumBufferSize() { return 5 * 1000 * 1000; }

}  // namespace felicia