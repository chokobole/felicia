#ifndef FELICIA_CORE_CHANNEL_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_H_

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/strcat.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/base/ip_endpoint.h"

#include "felicia/core/channel/channel_base.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/message/message_io.h"
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

template <typename MessageTy>
class TCPChannel;
template <typename MessageTy>
class UDPChannel;

template <typename MessageTy>
class Channel {
 public:
  virtual ~Channel() = default;

  virtual bool IsTCPChannel() const { return false; }
  virtual bool IsUDPChannel() const { return false; }

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

  virtual void Connect(const ChannelSource& channel_source,
                       StatusCallback callback) = 0;

  void SendMessage(const MessageTy& message, StatusCallback callback);
  void ReceiveMessage(MessageTy* message, StatusCallback callback);

 protected:
  friend class ChannelFactory;

  void OnSendMessage(const Status& s);
  void OnReceiveHeader(const Status& s);
  void OnReceiveMessage(const Status& s);
  void OnReceiveMessageWithHeader(const Status& s);

  Channel() {}

  Header header_;
  MessageTy* message_ = nullptr;

  std::unique_ptr<ChannelBase> channel_;
  std::vector<char> send_buffer_;
  StatusCallback send_callback_;
  std::vector<char> receive_buffer_;
  StatusCallback receive_callback_;

  DISALLOW_COPY_AND_ASSIGN(Channel);
};

template <typename MessageTy>
void Channel<MessageTy>::SendMessage(const MessageTy& message,
                                     StatusCallback callback) {
  DCHECK(channel_);
  DCHECK(this->send_callback_.is_null());
  DCHECK(!callback.is_null());

  if (send_buffer_.size() == 0) {
    send_buffer_.resize(ChannelBase::GetMaximumBufferSize());
  }

  size_t to_send;
  if (MessageIO<MessageTy>::SerializeToBuffer(&message, send_buffer_.data(),
                                              &to_send)) {
    DLOG(INFO) << "SendMessage() write bytes: " << to_send;
    this->send_callback_ = std::move(callback);
    channel_->Write(send_buffer_.data(), to_send,
                    ::base::BindOnce(&Channel<MessageTy>::OnSendMessage,
                                     ::base::Unretained(this)));
  } else {
    std::move(callback).Run(errors::InvalidArgument(::base::StrCat(
        {"message is invalid, which is", message.DebugString().c_str()})));
  }
}

template <typename MessageTy>
void Channel<MessageTy>::OnSendMessage(const Status& s) {
  std::move(this->send_callback_).Run(s);
}

template <typename MessageTy>
void Channel<MessageTy>::ReceiveMessage(MessageTy* message,
                                        StatusCallback callback) {
  DCHECK(channel_);
  DCHECK(this->receive_callback_.is_null());
  DCHECK(!callback.is_null());

  this->message_ = message;
  this->receive_callback_ = std::move(callback);

  if (receive_buffer_.size() == 0) {
    receive_buffer_.resize(ChannelBase::GetMaximumBufferSize());
  }

  if (channel_->IsTCPChannelBase()) {
    channel_->Read(receive_buffer_.data(), sizeof(Header),
                   ::base::BindOnce(&Channel<MessageTy>::OnReceiveHeader,
                                    ::base::Unretained(this)));
  } else {
    channel_->Read(
        receive_buffer_.data(), receive_buffer_.size(),
        ::base::BindOnce(&Channel<MessageTy>::OnReceiveMessageWithHeader,
                         ::base::Unretained(this)));
  }
}

template <typename MessageTy>
void Channel<MessageTy>::OnReceiveHeader(const Status& s) {
  DCHECK(channel_->IsTCPChannelBase());
  if (!s.ok()) {
    std::move(this->receive_callback_).Run(s);
    return;
  }

  if (!MessageIO<MessageTy>::ParseHeaderFromBuffer(receive_buffer_.data(),
                                                   &header_)) {
    std::move(this->receive_callback_)
        .Run(errors::DataLoss("Failed to parse header from buffer."));
    return;
  }

  channel_->Read(receive_buffer_.data(), header_.size(),
                 ::base::BindOnce(&Channel<MessageTy>::OnReceiveMessage,
                                  ::base::Unretained(this)));
}

template <typename MessageTy>
void Channel<MessageTy>::OnReceiveMessage(const Status& s) {
  DCHECK(channel_->IsTCPChannelBase());
  if (s.ok()) {
    if (!MessageIO<MessageTy>::ParseMessageFromBuffer(
            receive_buffer_.data(), header_, false, this->message_)) {
      std::move(this->receive_callback_)
          .Run(errors::DataLoss("Failed to parse message from buffer."));
      return;
    }
  }

  std::move(this->receive_callback_).Run(s);
}

template <typename MessageTy>
void Channel<MessageTy>::OnReceiveMessageWithHeader(const Status& s) {
  DCHECK(channel_->IsUDPChannelBase());
  if (!s.ok()) {
    std::move(this->receive_callback_).Run(s);
    return;
  }

  if (!MessageIO<MessageTy>::ParseHeaderFromBuffer(receive_buffer_.data(),
                                                   &header_)) {
    std::move(this->receive_callback_)
        .Run(errors::DataLoss("Failed to parse header from buffer."));
    return;
  }

  if (!MessageIO<MessageTy>::ParseMessageFromBuffer(
          receive_buffer_.data(), header_, true, this->message_)) {
    std::move(this->receive_callback_)
        .Run(errors::DataLoss("Failed to parse message from buffer."));
    return;
  }

  std::move(this->receive_callback_).Run(s);
}

// Convert ChannelSource |channel_source| to ::net::IPEndPoint,
// Retures true if succeeded.
EXPORT bool ToNetIPEndPoint(const ChannelSource& channel_source,
                            ::net::IPEndPoint* ip_endpoint);

// Convert ::net::IPEndPoint |ip_endpoint| to ChannelSource
EXPORT ChannelSource ToChannelSource(const ::net::IPEndPoint& ip_endpoint,
                                     ChannelDef_Type type);

// Convert ChannelDef |channel_def| to std::string
EXPORT std::string ToString(const ChannelDef& channel_def);

// Convert ChannelDef |channel_source| to std::string
EXPORT std::string ToString(const ChannelSource& channel_source);

// Randomly pick channel source.
EXPORT ChannelSource PickRandomChannelSource(ChannelDef_Type type);

// Check if |channel_source| is a valid. Returns true if so.
EXPORT bool IsValidChannelSource(const ChannelSource& channel_source);

// Check if |c| and |c2| are same. Returns true if so. Return false if
// either |c| or |c2| is invalid or they are not same.
EXPORT bool IsSameChannelSource(const ChannelSource& c,
                                const ChannelSource& c2);

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_H_