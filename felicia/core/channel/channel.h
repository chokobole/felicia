#ifndef FELICIA_CORE_CHANNEL_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_H_

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/base/ip_endpoint.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/master/master_data.pb.h"

namespace felicia {

using StatusOrIPEndPointCallback =
    ::base::OnceCallback<void(const StatusOr<::net::IPEndPoint>&)>;

template <typename MessageTy>
class TCPChannel;
template <typename MessageTy>
class UDPChannel;

template <typename MessageTy>
class EXPORT Channel {
 public:
  virtual ~Channel() = default;

  virtual bool IsTCPChannel() const { return false; }
  virtual bool IsUDPChannel() const { return false; }

  TCPChannel<MessageTy>* ToTCPChannel() {
    DCHECK(IsTCPChannel());
    return reinterpret_cast<TCPChannel<MessageTy>*>(this);
  }

  UDPChannel<MessageTy>* ToUDPChannel() {
    DCHECK(IsUDPChannel());
    return reinterpret_cast<UDPChannel<MessageTy>*>(this);
  }

  virtual void Connect(const NodeInfo& node_info, StatusCallback callback) = 0;
  virtual void Connect(const TopicSource& topic_source,
                       StatusCallback callback) = 0;

  virtual void SendMessage(const MessageTy& message,
                           StatusCallback callback) = 0;
  virtual void ReceiveMessage(MessageTy* message, StatusCallback callback) = 0;

 protected:
  friend class ChannelFactory;

  Channel() {}

  MessageTy* message_ = nullptr;
  StatusCallback receive_callback_;

  bool is_sending_ = false;
  bool is_receiving_ = false;

  DISALLOW_COPY_AND_ASSIGN(Channel);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_H_