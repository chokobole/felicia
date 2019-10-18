#ifndef FELICIA_CORE_CHANNEL_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_H_

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/build/build_config.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/base/ip_endpoint.h"

#include "felicia/core/channel/channel_buffer.h"
#include "felicia/core/channel/channel_impl.h"
#include "felicia/core/channel/socket/socket.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/lib/unit/bytes.h"
#include "felicia/core/message/message_io.h"
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

template <typename MessageTy>
class TCPChannel;
template <typename MessageTy>
class UDPChannel;
template <typename MessageTy>
class WSChannel;
template <typename MessageTy>
class UDSChannel;
template <typename MessageTy>
class ShmChannel;

template <typename MessageTy>
class Channel {
 public:
  virtual ~Channel() = default;

  virtual bool IsTCPChannel() const { return false; }
  virtual bool IsUDPChannel() const { return false; }
  virtual bool IsWSChannel() const { return false; }
  virtual bool IsUDSChannel() const { return false; }
  virtual bool IsShmChannel() const { return false; }

  virtual ChannelDef::Type type() const = 0;

  virtual bool HasReceivers() const { return true; }

  void set_use_ros_channel(bool use_ros_channel) {
    use_ros_channel_ = use_ros_channel;
    if (use_ros_channel) {
      header_size_ = 4;
    } else {
      header_size_ = sizeof(Header);
    }
  }
  bool use_ros_channel() const { return use_ros_channel_; }

  bool IsSendingMessage() const { return !send_callback_.is_null(); }
  bool IsReceivingMessage() const { return !receive_callback_.is_null(); }

  TCPChannel<MessageTy>* ToTCPChannel() {
    DCHECK(IsTCPChannel());
    return reinterpret_cast<TCPChannel<MessageTy>*>(this);
  }

  UDPChannel<MessageTy>* ToUDPChannel() {
    DCHECK(IsUDPChannel());
    return reinterpret_cast<UDPChannel<MessageTy>*>(this);
  }

  WSChannel<MessageTy>* ToWSChannel() {
    DCHECK(IsWSChannel());
    return reinterpret_cast<WSChannel<MessageTy>*>(this);
  }

  UDSChannel<MessageTy>* ToUDSChannel() {
    DCHECK(IsUDSChannel());
    return reinterpret_cast<UDSChannel<MessageTy>*>(this);
  }

  ShmChannel<MessageTy>* ToShmChannel() {
    DCHECK(IsShmChannel());
    return reinterpret_cast<ShmChannel<MessageTy>*>(this);
  }

  virtual void Connect(const ChannelDef& channel_def,
                       StatusOnceCallback callback) = 0;

  bool IsConnected() const {
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

  void SendMessage(const MessageTy& message, StatusOnceCallback callback);
  void SendRawMessage(const std::string& raw_message, bool reuse,
                      StatusOnceCallback callback);
  void ReceiveMessage(MessageTy* message, StatusOnceCallback callback);
  void ReceiveRawMessage(std::string* raw_message, StatusOnceCallback callback);

  void SetSendBuffer(const SendBuffer& send_buffer) {
    send_buffer_ = send_buffer;
  }

  virtual void SetSendBufferSize(Bytes bytes) {
    send_buffer_.SetCapacity(bytes);
  }
  virtual void SetReceiveBufferSize(Bytes bytes) {
    receive_buffer_.SetCapacity(bytes);
  }

  void SetDynamicSendBuffer(bool is_dynamic) {
    send_buffer_.SetDynamicBuffer(is_dynamic);
  }
  void SetDynamicReceiveBuffer(bool is_dynamic) {
    receive_buffer_.SetDynamicBuffer(is_dynamic);
  }

 protected:
  friend class ChannelFactory;

  virtual void WriteImpl(const std::string& text, StatusOnceCallback callback);
  virtual void ReadImpl(MessageTy* message, StatusOnceCallback callback);
  virtual void ReadRawImpl(std::string* raw_message,
                           StatusOnceCallback callback);

  void OnSendMessage(const Status& s);
  void OnReceiveHeader(const Status& s);
  void OnReceiveMessage(const Status& s);

  Channel() {}

  Header header_;
  MessageTy* message_ = nullptr;
  std::string* raw_message_ = nullptr;

  std::unique_ptr<ChannelImpl> channel_impl_;
  SendBuffer send_buffer_;
  StatusOnceCallback send_callback_;
  ChannelBuffer receive_buffer_;
  StatusOnceCallback receive_callback_;

  bool use_ros_channel_ = false;
  size_t header_size_ = sizeof(Header);

  DISALLOW_COPY_AND_ASSIGN(Channel);
};

template <typename MessageTy>
void Channel<MessageTy>::SendMessage(const MessageTy& message,
                                     StatusOnceCallback callback) {
  DCHECK(channel_impl_);
  DCHECK(send_callback_.is_null());
  DCHECK(!callback.is_null());

  send_buffer_.Reset();
  std::string text;
  MessageIOError err = MessageIO::SerializeToString(&message, &text);
  if (err == MessageIOError::OK) {
    send_buffer_.InvalidateAttachment();
    WriteImpl(text, std::move(callback));
  } else {
    if (!callback.is_null())
      std::move(callback).Run(errors::Unavailable(MessageIOErrorToString(err)));
  }
}

template <typename MessageTy>
void Channel<MessageTy>::SendRawMessage(const std::string& raw_message,
                                        bool reuse,
                                        StatusOnceCallback callback) {
  DCHECK(channel_impl_);
  DCHECK(send_callback_.is_null());
  DCHECK(!callback.is_null());

  send_buffer_.Reset();
  if (!reuse) {
    send_buffer_.InvalidateAttachment();
  }
  WriteImpl(raw_message, std::move(callback));
}

template <typename MessageTy>
void Channel<MessageTy>::OnSendMessage(const Status& s) {
  std::move(send_callback_).Run(s);
}

template <typename MessageTy>
void Channel<MessageTy>::ReceiveMessage(MessageTy* message,
                                        StatusOnceCallback callback) {
  DCHECK(channel_impl_);
  DCHECK(receive_callback_.is_null());
  DCHECK(!callback.is_null());

  receive_buffer_.Reset();
  ReadImpl(message, std::move(callback));
}

template <typename MessageTy>
void Channel<MessageTy>::ReceiveRawMessage(std::string* raw_message,
                                           StatusOnceCallback callback) {
  DCHECK(channel_impl_);
  DCHECK(receive_callback_.is_null());
  DCHECK(!callback.is_null());

  receive_buffer_.Reset();
  ReadRawImpl(raw_message, std::move(callback));
}

template <typename MessageTy>
void Channel<MessageTy>::WriteImpl(const std::string& text,
                                   StatusOnceCallback callback) {
  MessageIOError err = MessageIOError::OK;
  if (!send_buffer_.CanReuse(SendBuffer::ATTACH_KIND_GENERAL)) {
    int to_send;
    err = MessageIO::AttachToBuffer(text, send_buffer_.buffer(), header_size_,
                                    &to_send);
    if (err == MessageIOError::ERR_NOT_ENOUGH_BUFFER) {
      if (send_buffer_.SetEnoughCapacityIfDynamic(to_send)) {
        err = MessageIO::AttachToBuffer(text, send_buffer_.buffer(),
                                        header_size_, &to_send);
      }
    }
    send_buffer_.AttachGeneral(to_send);
  }

  if (err == MessageIOError::OK) {
    send_callback_ = std::move(callback);
    channel_impl_->WriteAsync(send_buffer_.buffer(), send_buffer_.size(),
                              base::BindOnce(&Channel<MessageTy>::OnSendMessage,
                                             base::Unretained(this)));
  } else {
    if (!callback.is_null())
      std::move(callback).Run(errors::Unavailable(MessageIOErrorToString(err)));
  }
}

template <typename MessageTy>
void Channel<MessageTy>::ReadImpl(MessageTy* message,
                                  StatusOnceCallback callback) {
  receive_buffer_.SetEnoughCapacityIfDynamic(header_size_);

  message_ = message;
  receive_callback_ = std::move(callback);
  channel_impl_->ReadAsync(receive_buffer_.buffer(), header_size_,
                           base::BindOnce(&Channel<MessageTy>::OnReceiveHeader,
                                          base::Unretained(this)));
}

template <typename MessageTy>
void Channel<MessageTy>::ReadRawImpl(std::string* raw_message,
                                     StatusOnceCallback callback) {
  receive_buffer_.SetEnoughCapacityIfDynamic(header_size_);

  raw_message_ = raw_message;
  receive_callback_ = std::move(callback);
  channel_impl_->ReadAsync(receive_buffer_.buffer(), header_size_,
                           base::BindOnce(&Channel<MessageTy>::OnReceiveHeader,
                                          base::Unretained(this)));
}

template <typename MessageTy>
void Channel<MessageTy>::OnReceiveHeader(const Status& s) {
  DCHECK(!IsUDPChannel());
  if (!s.ok()) {
    std::move(receive_callback_).Run(s);
    return;
  }

  MessageIOError err = MessageIO::ParseHeaderFromBuffer(
      receive_buffer_.StartOfBuffer(), &header_, use_ros_channel_);
  if (err != MessageIOError::OK) {
    std::move(receive_callback_)
        .Run(errors::DataLoss(MessageIOErrorToString(err)));
    return;
  }

  int bytes = header_size_ + header_.size();
  if (!receive_buffer_.SetEnoughCapacityIfDynamic(bytes)) {
    std::move(receive_callback_)
        .Run(errors::Aborted(
            MessageIOErrorToString(MessageIOError::ERR_NOT_ENOUGH_BUFFER)));
    return;
  }

  receive_buffer_.set_offset(0);
  channel_impl_->ReadAsync(receive_buffer_.buffer(), header_.size(),
                           base::BindOnce(&Channel<MessageTy>::OnReceiveMessage,
                                          base::Unretained(this)));
}

template <typename MessageTy>
void Channel<MessageTy>::OnReceiveMessage(const Status& s) {
  if (s.ok()) {
    if (message_) {
      MessageIOError err = MessageIO::ParseMessageFromBuffer(
          receive_buffer_.StartOfBuffer(), header_, 0, message_);
      if (err != MessageIOError::OK) {
        std::move(receive_callback_)
            .Run(errors::DataLoss("Failed to parse message from buffer."));
        return;
      }
    } else {
      CHECK(raw_message_);
      *raw_message_ =
          std::string(receive_buffer_.StartOfBuffer(), header_.size());
    }
  }

  std::move(receive_callback_).Run(s);
}

// Convert |ip_endpoint()| of |channel_def| to net::IPEndPoint,
// Returns Status::OK() if succeeded.
EXPORT Status ToNetIPEndPoint(const ChannelDef& channel_def,
                              net::IPEndPoint* ip_endpoint);

// Convert EndPoint of |channel_def| to std::string
EXPORT std::string EndPointToString(const ChannelDef& channel_def);

// Check if |channel_def| is a valid. Returns true if so.
EXPORT bool IsValidChannelDef(const ChannelDef& channel_def);

// Check if |channel_source| is a valid. Returns true if so.
EXPORT bool IsValidChannelSource(const ChannelSource& channel_source);

// Check if |c| and |c2| are same. Returns true if so. Return false if
// either |c| or |c2| is invalid or they are not same.
EXPORT bool IsSameChannelDef(const ChannelDef& c, const ChannelDef& c2);

// Check if |c| and |c2| are same. Returns true if so. Return false if
// either |c| or |c2| is invalid or they are not same.
EXPORT bool IsSameChannelSource(const ChannelSource& c,
                                const ChannelSource& c2);

EXPORT int AllChannelTypes();

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_H_