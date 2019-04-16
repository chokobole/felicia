#include "felicia/core/channel/udp_channel_base.h"

#include "third_party/chromium/net/base/net_errors.h"

namespace felicia {

UDPChannelBase::UDPChannelBase() = default;
UDPChannelBase::~UDPChannelBase() = default;

bool UDPChannelBase::IsUDPChannelBase() const { return true; }

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

void UDPChannelBase::OnReadAsync(
    char* buffer, scoped_refptr<::net::IOBufferWithSize> read_buffer,
    int result) {
  if (result > 0) memcpy(buffer, read_buffer->data(), result);
  OnRead(result);
}

void UDPChannelBase::OnRead(int result) {
  if (result == ::net::ERR_MSG_TOO_BIG) {
    LOG(ERROR) << "Msg too big";
    CallbackWithStatus(std::move(read_callback_), 0);
    return;
  }
  CallbackWithStatus(std::move(read_callback_), result);
}

}  // namespace felicia