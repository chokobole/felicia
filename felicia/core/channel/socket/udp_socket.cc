#include "felicia/core/channel/socket/udp_socket.h"

namespace felicia {

UDPSocket::UDPSocket() = default;
UDPSocket::~UDPSocket() = default;

bool UDPSocket::IsUDPSocket() const { return true; }

UDPClientSocket* UDPSocket::ToUDPClientSocket() {
  DCHECK(IsClient());
  return reinterpret_cast<UDPClientSocket*>(this);
}

UDPServerSocket* UDPSocket::ToUDPServerSocket() {
  DCHECK(IsServer());
  return reinterpret_cast<UDPServerSocket*>(this);
}

void UDPSocket::OnWrite(int result) {
  CallbackWithStatus(std::move(write_callback_), result);
}

void UDPSocket::OnReadAsync(char* buffer,
                            scoped_refptr<::net::IOBufferWithSize> read_buffer,
                            int result) {
  if (result > 0) memcpy(buffer, read_buffer->data(), result);
  OnRead(result);
}

void UDPSocket::OnRead(int result) {
  CallbackWithStatus(std::move(read_callback_), result);
}

}  // namespace felicia