#ifndef FELICIA_CORE_CHANNEL_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_H_

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/build/build_config.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/base/ip_endpoint.h"

#include "felicia/core/channel/channel_impl.h"
#include "felicia/core/channel/socket/socket.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/lib/unit/bytes.h"
#include "felicia/core/message/message_io.h"
#include "felicia/core/protobuf/channel.pb.h"
#if defined(OS_POSIX)
#include "felicia/core/channel/socket/uds_endpoint.h"
#endif

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

using SendMessageCallback =
    ::base::RepeatingCallback<void(ChannelDef::Type, const Status&)>;

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

  bool IsSendingMessage() const { return is_sending_; }
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

  void SendMessage(const MessageTy& message, SendMessageCallback callback);
  void ReceiveMessage(MessageTy* message, StatusOnceCallback callback);

  virtual void SetSendBufferSize(Bytes bytes) {
    send_buffer_->SetCapacity(bytes.bytes());
  }
  virtual void SetReceiveBufferSize(Bytes bytes) {
    receive_buffer_->SetCapacity(bytes.bytes());
  }

  void EnableDynamicBuffer() { is_dynamic_buffer_ = true; }

 protected:
  friend class ChannelFactory;

  virtual void WriteImpl(const std::string& text, SendMessageCallback callback);
  virtual void ReadImpl(MessageTy* message, StatusOnceCallback callback);

  void OnSendMessage(const Status& s);
  void OnReceiveHeader(const Status& s);
  void OnReceiveMessage(const Status& s);

  Channel()
      : send_buffer_(::base::MakeRefCounted<::net::GrowableIOBuffer>()),
        receive_buffer_(::base::MakeRefCounted<::net::GrowableIOBuffer>()) {}

  Header header_;
  MessageTy* message_ = nullptr;

  std::unique_ptr<ChannelImpl> channel_impl_;
  scoped_refptr<::net::GrowableIOBuffer> send_buffer_;
  SendMessageCallback send_callback_;
  scoped_refptr<::net::GrowableIOBuffer> receive_buffer_;
  StatusOnceCallback receive_callback_;

  bool is_sending_ = false;
  bool is_dynamic_buffer_ = false;

  DISALLOW_COPY_AND_ASSIGN(Channel);
};

template <typename MessageTy>
void Channel<MessageTy>::SendMessage(const MessageTy& message,
                                     SendMessageCallback callback) {
  DCHECK(channel_impl_);
  DCHECK(!is_sending_);
  DCHECK(!callback.is_null());

  if (!is_dynamic_buffer_ && send_buffer_->capacity() == 0) {
    DLOG(WARNING) << "Send buffer was not allocated, used default size.";
    send_buffer_->SetCapacity(Bytes::FromKilloBytes(1).bytes());
  }

  send_buffer_->set_offset(0);
  std::string text;
  MessageIoError err = MessageIO<MessageTy>::SerializeToString(&message, &text);
  if (err == MessageIoError::OK) {
    WriteImpl(text, callback);
  } else {
    callback.Run(type(), errors::Unavailable(MessageIoErrorToString(err)));
  }
}

template <typename MessageTy>
void Channel<MessageTy>::OnSendMessage(const Status& s) {
  is_sending_ = false;
  send_callback_.Run(type(), s);
}

template <typename MessageTy>
void Channel<MessageTy>::ReceiveMessage(MessageTy* message,
                                        StatusOnceCallback callback) {
  DCHECK(channel_impl_);
  DCHECK(receive_callback_.is_null());
  DCHECK(!callback.is_null());

  if (!is_dynamic_buffer_ && receive_buffer_->capacity() == 0) {
    DLOG(WARNING) << "Receive buffer was not allocated, used default size.";
    receive_buffer_->SetCapacity(Bytes::FromKilloBytes(1).bytes());
  }

  receive_buffer_->set_offset(0);
  ReadImpl(message, std::move(callback));
}

template <typename MessageTy>
void Channel<MessageTy>::WriteImpl(const std::string& text,
                                   SendMessageCallback callback) {
  int to_send;
  MessageIoError err =
      MessageIO<MessageTy>::AttachToBuffer(text, send_buffer_, &to_send);
  if (err == MessageIoError::ERR_NOT_ENOUGH_BUFFER) {
    if (is_dynamic_buffer_) {
      DLOG(INFO) << "Dynamically allocate buffer " << Bytes::FromBytes(to_send);
      send_buffer_->SetCapacity(to_send);
      err = MessageIO<MessageTy>::AttachToBuffer(text, send_buffer_, &to_send);
    }
  }

  if (err == MessageIoError::OK) {
    is_sending_ = true;
    send_callback_ = callback;
    channel_impl_->Write(send_buffer_, to_send,
                         ::base::BindOnce(&Channel<MessageTy>::OnSendMessage,
                                          ::base::Unretained(this)));
  } else {
    callback.Run(type(), errors::Unavailable(MessageIoErrorToString(err)));
  }
}

template <typename MessageTy>
void Channel<MessageTy>::ReadImpl(MessageTy* message,
                                  StatusOnceCallback callback) {
  if (is_dynamic_buffer_ && receive_buffer_->capacity() == 0) {
    receive_buffer_->SetCapacity(sizeof(Header));
  }

  message_ = message;
  receive_callback_ = std::move(callback);
  channel_impl_->Read(receive_buffer_, sizeof(Header),
                      ::base::BindOnce(&Channel<MessageTy>::OnReceiveHeader,
                                       ::base::Unretained(this)));
}

template <typename MessageTy>
void Channel<MessageTy>::OnReceiveHeader(const Status& s) {
  DCHECK(!IsUDPChannel());
  if (!s.ok()) {
    std::move(receive_callback_).Run(s);
    return;
  }

  MessageIoError err = MessageIO<MessageTy>::ParseHeaderFromBuffer(
      receive_buffer_->StartOfBuffer(), &header_);
  if (err != MessageIoError::OK) {
    std::move(receive_callback_)
        .Run(errors::DataLoss(MessageIoErrorToString(err)));
    return;
  }

  int bytes = sizeof(Header) + header_.size();
  if (receive_buffer_->capacity() < bytes) {
    if (is_dynamic_buffer_) {
      DLOG(INFO) << "Dynamically allocate buffer " << Bytes::FromBytes(bytes);
      receive_buffer_->SetCapacity(bytes);
    } else {
      std::move(receive_callback_)
          .Run(errors::Aborted(
              MessageIoErrorToString(MessageIoError::ERR_NOT_ENOUGH_BUFFER)));
      return;
    }
  }

  receive_buffer_->set_offset(0);
  channel_impl_->Read(receive_buffer_, header_.size(),
                      ::base::BindOnce(&Channel<MessageTy>::OnReceiveMessage,
                                       ::base::Unretained(this)));
}

template <typename MessageTy>
void Channel<MessageTy>::OnReceiveMessage(const Status& s) {
  if (s.ok()) {
    MessageIoError err = MessageIO<MessageTy>::ParseMessageFromBuffer(
        receive_buffer_->StartOfBuffer(), header_, false, message_);
    if (err != MessageIoError::OK) {
      std::move(receive_callback_)
          .Run(errors::DataLoss("Failed to parse message from buffer."));
      return;
    }
  }

  std::move(receive_callback_).Run(s);
}

// Convert |ip_endpoint()| of |channel_def| to ::net::IPEndPoint,
// Returns Status::OK() if succeeded.
EXPORT Status ToNetIPEndPoint(const ChannelDef& channel_def,
                              ::net::IPEndPoint* ip_endpoint);

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

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_H_