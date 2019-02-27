#ifndef FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_

#include <memory>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/strcat.h"
#include "third_party/chromium/net/base/ip_endpoint.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/tcp_client_channel.h"
#include "felicia/core/channel/tcp_server_channel.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/master_data.pb.h"
#include "felicia/core/message/message.h"

namespace felicia {

template <typename MessageTy>
class TCPChannel : public Channel<MessageTy> {
 public:
  TCPChannel();
  ~TCPChannel();

  bool IsTCPChannel() const override { return true; }

  void Listen(const NodeInfo& node_info,
              StatusOrIPEndPointCallback listen_callback,
              StatusCallback accept_callback);

  void Connect(const NodeInfo& node_info, StatusCallback callback) override;
  void Connect(const TopicSource& topic_source,
               StatusCallback callback) override;

  void SendMessage(const MessageTy& message, StatusCallback callback) override;

  void ReceiveMessage(MessageTy* message, StatusCallback callback) override;

 private:
  void DoConnect(const IPEndPoint& ip_endpoint, StatusCallback callback);

  void OnReadHeader(const scoped_refptr<::net::IOBufferWithSize>& buffer,
                    const Status& s);
  void OnReadContent(const scoped_refptr<::net::IOBufferWithSize>& buffer,
                     const Status& s);

  std::unique_ptr<TCPChannelBase> tcp_channel_;

  DISALLOW_COPY_AND_ASSIGN(TCPChannel);
};

template <typename MessageTy>
TCPChannel<MessageTy>::TCPChannel() {}

template <typename MessageTy>
TCPChannel<MessageTy>::~TCPChannel() = default;

template <typename MessageTy>
void TCPChannel<MessageTy>::Listen(const NodeInfo& node_info,
                                   StatusOrIPEndPointCallback listen_callback,
                                   StatusCallback accept_callback) {
  DCHECK(!tcp_channel_);
  tcp_channel_ = std::make_unique<TCPServerChannel>();
  TCPServerChannel* server_channel = tcp_channel_->ToTCPServerChannel();
  server_channel->set_accept_callback(accept_callback);
  server_channel->Listen(node_info, std::move(listen_callback));
}

template <typename MessageTy>
void TCPChannel<MessageTy>::Connect(const NodeInfo& node_info,
                                    StatusCallback callback) {
  DoConnect(node_info.ip_endpoint(), callback);
}

template <typename MessageTy>
void TCPChannel<MessageTy>::Connect(const TopicSource& topic_source,
                                    StatusCallback callback) {
  DoConnect(topic_source.topic_ip_endpoint(), callback);
}

template <typename MessageTy>
void TCPChannel<MessageTy>::DoConnect(const IPEndPoint& ip_endpoint,
                                      StatusCallback callback) {
  tcp_channel_ = std::make_unique<TCPClientChannel>();
  ::net::IPAddress address;
  bool ret = address.AssignFromIPLiteral(ip_endpoint.ip());
  DCHECK(ret);
  ::net::IPEndPoint net_ip_endpoint(address,
                                    static_cast<uint16_t>(ip_endpoint.port()));
  tcp_channel_->ToTCPClientChannel()->Connect(net_ip_endpoint, callback);
}

template <typename MessageTy>
void TCPChannel<MessageTy>::SendMessage(const MessageTy& message,
                                        StatusCallback callback) {
  DCHECK(!this->is_sending_);
  DCHECK(callback);
  this->is_sending_ = true;
  scoped_refptr<::net::IOBufferWithSize> buffer;
  if (Message<MessageTy>::SerializeToBuffer(&message, &buffer)) {
    LOG(INFO) << "SendMessage() write bytes: " << buffer->size();
    tcp_channel_->Write(buffer.get(), buffer->size(),
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
void TCPChannel<MessageTy>::ReceiveMessage(MessageTy* message,
                                           StatusCallback callback) {
  DCHECK(!this->is_receiving_);
  DCHECK(callback);
  scoped_refptr<::net::IOBufferWithSize> buffer =
      ::base::MakeRefCounted<::net::IOBufferWithSize>(
          ChannelBase::kMaxReceiverBufferSize);
  this->is_receiving_ = true;
  this->message_ = message;
  this->receive_callback_ = callback;
  tcp_channel_->Read(
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

#endif  // FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_