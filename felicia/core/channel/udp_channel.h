#ifndef FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_

#include <memory>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/strcat.h"
#include "third_party/chromium/net/base/ip_endpoint.h"
#include "third_party/chromium/net/base/net_errors.h"
#include "third_party/chromium/net/socket/udp_socket.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/udp_client_channel.h"
#include "felicia/core/channel/udp_server_channel.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/master_data.pb.h"
#include "felicia/core/message/message.h"

namespace felicia {

template <typename MessageTy>
class UDPChannel : public Channel<MessageTy> {
 public:
  UDPChannel();
  ~UDPChannel();

  bool IsUDPChannel() const override { return true; }

  void Bind(const NodeInfo& node_info, StatusOrIPEndPointCallback callback);

  void Connect(const NodeInfo& node_info, StatusCallback callback) override;
  void Connect(const TopicSource& topic_source,
               StatusCallback callback) override;

  void SendMessage(const MessageTy& message, StatusCallback callback) override;

  void ReceiveMessage(MessageTy* message, StatusCallback callback) override;

 private:
  void DoConnect(const IPEndPoint& ip_endpoint, StatusCallback callback);

  std::unique_ptr<UDPChannelBase> udp_channel_;

  DISALLOW_COPY_AND_ASSIGN(UDPChannel);
};

template <typename MessageTy>
UDPChannel<MessageTy>::UDPChannel() {}

template <typename MessageTy>
UDPChannel<MessageTy>::~UDPChannel() {}

template <typename MessageTy>
void UDPChannel<MessageTy>::Bind(const NodeInfo& node_info,
                                 StatusOrIPEndPointCallback callback) {
  DCHECK(!udp_channel_);
  udp_channel_ = std::make_unique<UDPServerChannel>();
  udp_channel_->ToUDPServerChannel()->Bind(node_info, std::move(callback));
}

template <typename MessageTy>
void UDPChannel<MessageTy>::Connect(const NodeInfo& node_info,
                                    StatusCallback callback) {
  DoConnect(node_info.ip_endpoint(), callback);
}

template <typename MessageTy>
void UDPChannel<MessageTy>::Connect(const TopicSource& topic_source,
                                    StatusCallback callback) {
  DoConnect(topic_source.topic_ip_endpoint(), callback);
}

template <typename MessageTy>
void UDPChannel<MessageTy>::DoConnect(const IPEndPoint& ip_endpoint,
                                      StatusCallback callback) {
  DCHECK(!udp_channel_);
  udp_channel_ = std::make_unique<UDPClientChannel>();
  ::net::IPAddress address;
  bool ret = address.AssignFromIPLiteral(ip_endpoint.ip());
  DCHECK(ret);
  ::net::IPEndPoint net_ip_endpoint(address,
                                    static_cast<uint16_t>(ip_endpoint.port()));
  udp_channel_->ToUDPClientChannel()->Connect(net_ip_endpoint, callback);
}

template <typename MessageTy>
void UDPChannel<MessageTy>::SendMessage(const MessageTy& message,
                                        StatusCallback callback) {
  DCHECK(!this->is_sending_);
  DCHECK(callback);
  this->is_sending_ = true;
  scoped_refptr<::net::IOBufferWithSize> buffer;
  if (Message<MessageTy>::SerializeToBuffer(&message, &buffer)) {
    LOG(INFO) << "SendMessage() write bytes: " << buffer->size();
    udp_channel_->Write(buffer.get(), buffer->size(),
                        [this, callback](const Status& s) {
                          this->is_sending_ = false;
                          callback(s);
                        });
  } else {
    this->is_sending_ = false;
    callback(errors::InvalidArgument(::base::StrCat(
        {"message is invalid, which is", message.DebugString().c_str()})));
  }
}

template <typename MessageTy>
void UDPChannel<MessageTy>::ReceiveMessage(MessageTy* message,
                                           StatusCallback callback) {
  DCHECK(!this->is_receiving_);
  DCHECK(callback);
  scoped_refptr<::net::IOBufferWithSize> buffer =
      ::base::MakeRefCounted<::net::IOBufferWithSize>(
          ChannelBase::kMaxReceiverBufferSize);
  this->is_receiving_ = true;
  this->message_ = message;
  this->receive_callback_ = callback;
  udp_channel_->Read(
      buffer.get(), buffer->size(), [this, buffer, message](const Status& s) {
        if (s.ok()) {
          bool ret = Message<MessageTy>::ParseFromBuffer(buffer, message);
          if (!ret) {
            this->is_receiving_ = false;
            this->receive_callback_(
                errors::DataLoss("Failed to parse from buffer"));
            return;
          }
        }

        this->is_receiving_ = false;
        this->receive_callback_(s);
      });
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_