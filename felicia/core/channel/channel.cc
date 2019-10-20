#include "felicia/core/channel/channel.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/build/build_config.h"

#include "felicia/core/channel/socket/socket.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/net/net_util.h"
#include "felicia/core/message/message_io_error.h"
#if defined(OS_POSIX)
#include "felicia/core/channel/socket/uds_endpoint.h"
#endif

namespace felicia {

Channel::Channel() = default;
Channel::~Channel() = default;

bool Channel::IsShmChannel() const { return false; }
bool Channel::IsTCPChannel() const { return false; }
bool Channel::IsUDPChannel() const { return false; }
bool Channel::IsUDSChannel() const { return false; }
bool Channel::IsWSChannel() const { return false; }

ShmChannel* Channel::ToShmChannel() {
  DCHECK(IsShmChannel());
  return reinterpret_cast<ShmChannel*>(this);
}

TCPChannel* Channel::ToTCPChannel() {
  DCHECK(IsTCPChannel());
  return reinterpret_cast<TCPChannel*>(this);
}

UDPChannel* Channel::ToUDPChannel() {
  DCHECK(IsUDPChannel());
  return reinterpret_cast<UDPChannel*>(this);
}

UDSChannel* Channel::ToUDSChannel() {
  DCHECK(IsUDSChannel());
  return reinterpret_cast<UDSChannel*>(this);
}

WSChannel* Channel::ToWSChannel() {
  DCHECK(IsWSChannel());
  return reinterpret_cast<WSChannel*>(this);
}

bool Channel::HasReceivers() const { return true; }

bool Channel::ShouldReceiveMessageWithHeader() const { return false; }

bool Channel::HasNativeHeader() const { return false; }

bool Channel::IsSending() const { return !send_callback_.is_null(); }
bool Channel::IsReceiving() const { return !receive_callback_.is_null(); }

bool Channel::IsConnected() const {
  DCHECK(this->channel_impl_);
  if (this->channel_impl_->IsSocket()) {
    Socket* socket = this->channel_impl_->ToSocket();
    return socket->IsConnected();
  } else if (this->channel_impl_->IsSharedMemory()) {
    return true;
  }
  NOTREACHED();
  return true;
}

void Channel::SetSendBuffer(const ChannelBuffer& send_buffer) {
  send_buffer_ = send_buffer;
}

void Channel::SetReceiveBuffer(const ChannelBuffer& receive_buffer) {
  receive_buffer_ = receive_buffer;
}

void Channel::SetSendBufferSize(Bytes bytes) {
  send_buffer_.SetCapacity(bytes);
}

void Channel::SetReceiveBufferSize(Bytes bytes) {
  receive_buffer_.SetCapacity(bytes);
}

void Channel::SetDynamicSendBuffer(bool is_dynamic) {
  send_buffer_.SetDynamicBuffer(is_dynamic);
}

void Channel::SetDynamicReceiveBuffer(bool is_dynamic) {
  receive_buffer_.SetDynamicBuffer(is_dynamic);
}

void Channel::Send(const std::string& text, StatusOnceCallback callback) {
  send_buffer_.Reset();
  int to_send = text.length();
  if (!send_buffer_.SetEnoughCapacityIfDynamic(to_send)) {
    std::move(callback).Run(errors::Aborted(
        MessageIOErrorToString(MessageIOError::ERR_NOT_ENOUGH_BUFFER)));
    return;
  }

  memcpy(send_buffer_.StartOfBuffer(), text.c_str(), to_send);

  SendInternalBuffer(to_send, std::move(callback));
}

void Channel::Receive(std::string* text, StatusOnceCallback callback) {
  text_ = text;
  ReceiveInternalBuffer(text->length(), std::move(callback));
}

void Channel::SendInternalBuffer(int size, StatusOnceCallback callback) {
  DCHECK(channel_impl_);
  DCHECK(send_callback_.is_null());
  DCHECK(!callback.is_null());
  DCHECK_GT(size, 0);

  send_callback_ = std::move(callback);
  channel_impl_->WriteAsync(
      send_buffer_.buffer(), size,
      base::BindOnce(&Channel::OnSend, base::Unretained(this)));
}

void Channel::ReceiveInternalBuffer(int size, StatusOnceCallback callback) {
  DCHECK(channel_impl_);
  DCHECK(receive_callback_.is_null());
  DCHECK(!callback.is_null());

  receive_buffer_.Reset();
  if (!TrySetEnoughReceiveBufferSize(size)) {
    std::move(callback).Run(errors::Aborted(
        MessageIOErrorToString(MessageIOError::ERR_NOT_ENOUGH_BUFFER)));
    return;
  }
  int to_receive;
  if (ShouldReceiveMessageWithHeader()) {
    to_receive = receive_buffer_.capacity();
  } else {
    to_receive = size;
  }
  DCHECK_GT(to_receive, 0);

  receive_callback_ = std::move(callback);
  channel_impl_->ReadAsync(
      receive_buffer_.buffer(), to_receive,
      base::BindOnce(&Channel::OnReceive, base::Unretained(this)));
}

void Channel::OnSend(const Status& s) { std::move(send_callback_).Run(s); }

void Channel::OnReceive(const Status& s) {
  if (text_) {
    memcpy(const_cast<char*>(text_->c_str()), receive_buffer_.StartOfBuffer(),
           text_->length());
    text_ = nullptr;
  }
  std::move(receive_callback_).Run(s);
}

bool Channel::TrySetEnoughReceiveBufferSize(int capacity) {
  return receive_buffer_.SetEnoughCapacityIfDynamic(capacity);
}

Status ToNetIPEndPoint(const ChannelDef& channel_def,
                       net::IPEndPoint* ip_endpoint) {
  if (!channel_def.has_ip_endpoint()) {
    return errors::InvalidArgument(
        "channel_def doesn't contain an IPEndPoint.");
  }

  net::IPAddress ip;
  if (!ip.AssignFromIPLiteral(channel_def.ip_endpoint().ip())) {
    return errors::InvalidArgument("Failed to convert to IPAddress.");
  }

  *ip_endpoint = net::IPEndPoint(ip, channel_def.ip_endpoint().port());
  return Status::OK();
}

std::string EndPointToString(const ChannelDef& channel_def) {
  if (channel_def.has_ip_endpoint()) {
    net::IPEndPoint ip_endpoint;
    if (ToNetIPEndPoint(channel_def, &ip_endpoint).ok()) {
      return ip_endpoint.ToString();
    }
  } else if (channel_def.has_uds_endpoint()) {
#if defined(OS_POSIX)
    net::UDSEndPoint uds_endpoint;
    if (ToNetUDSEndPoint(channel_def, &uds_endpoint).ok()) {
      return uds_endpoint.ToString();
    }
#endif
  } else if (channel_def.has_shm_endpoint()) {
#if defined(OS_WIN)
    return base::EmptyString();
#elif defined(OS_MACOSX)
    return channel_def.shm_endpoint().broker_endpoint().service_name();
#else
    net::UDSEndPoint uds_endpoint;
    if (ToNetUDSEndPoint(channel_def.shm_endpoint().broker_endpoint(),
                         &uds_endpoint)
            .ok()) {
      return uds_endpoint.ToString();
    }
#endif
    return channel_def.shm_endpoint().DebugString();
  }

  return base::EmptyString();
}

bool IsValidChannelDef(const ChannelDef& channel_def) {
  ChannelDef::Type type = channel_def.type();
  if (type == ChannelDef::CHANNEL_TYPE_TCP ||
      type == ChannelDef::CHANNEL_TYPE_UDP ||
      type == ChannelDef::CHANNEL_TYPE_WS) {
    net::IPEndPoint ip_endpoint;
    return ToNetIPEndPoint(channel_def, &ip_endpoint).ok();
  } else if (type == ChannelDef::CHANNEL_TYPE_UDS) {
#if defined(OS_POSIX)
    net::UDSEndPoint uds_endpoint;
    return ToNetUDSEndPoint(channel_def, &uds_endpoint).ok();
#endif
  } else if (type == ChannelDef::CHANNEL_TYPE_SHM) {
    return true;
  }
  return false;
}

bool IsValidChannelSource(const ChannelSource& channel_source) {
  if (channel_source.channel_defs_size() == 0) return false;

  for (auto& channel_def : channel_source.channel_defs()) {
    if (!IsValidChannelDef(channel_def)) return false;
  }
  return true;
}

bool IsSameChannelDef(const ChannelDef& c, const ChannelDef& c2) {
  net::IPEndPoint ip_endpoint;
  if (!ToNetIPEndPoint(c, &ip_endpoint).ok()) return false;
  net::IPEndPoint ip_endpoint2;
  if (!ToNetIPEndPoint(c2, &ip_endpoint2).ok()) return false;

  return ip_endpoint == ip_endpoint2;
}

bool IsSameChannelSource(const ChannelSource& c, const ChannelSource& c2) {
  if (c.channel_defs_size() != c2.channel_defs_size()) return false;

  for (int i = 0; i < c.channel_defs_size(); ++i) {
    if (!IsSameChannelDef(c.channel_defs(i), c2.channel_defs(i))) return false;
  }
  return true;
}

int AllChannelTypes() {
  int channel_types = 0;
  for (int i = 1; i < ChannelDef_Type_Type_ARRAYSIZE; i = i << 1) {
    channel_types |= i;
  }
  return channel_types;
}

}  // namespace felicia