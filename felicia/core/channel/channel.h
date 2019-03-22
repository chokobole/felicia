#ifndef FELICIA_CORE_CHANNEL_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_H_

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/base/ip_endpoint.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/statusor.h"
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

  virtual void SendMessage(const MessageTy& message,
                           StatusCallback callback) = 0;
  virtual void ReceiveMessage(MessageTy* message, StatusCallback callback) = 0;

 protected:
  friend class ChannelFactory;

  Channel() {}

  MessageTy* message_ = nullptr;

  StatusCallback send_callback_;
  StatusCallback receive_callback_;

  DISALLOW_COPY_AND_ASSIGN(Channel);
};

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