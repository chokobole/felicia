#ifndef FELICIA_CC_COMMUNICATION_PUBLISHER_H_
#define FELICIA_CC_COMMUNICATION_PUBLISHER_H_

#include <functional>
#include <memory>
#include <string>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/cc/client_node.h"
#include "felicia/cc/master_proxy.h"
#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/containers/pool.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

template <typename MessageTy>
class EXPORT Publisher {
 public:
  using OnMessageCallback = std::function<MessageTy(void)>;

  explicit Publisher(ClientNode* client_node) : client_node_(client_node) {}

  void Publish(::base::StringPiece topic, OnMessageCallback on_message_callback,
               const ChannelDef& channel_def = ChannelDef());
  void RequestPublish(const ChannelDef& channel_def,
                      const StatusOr<::net::IPEndPoint>& status_or);

 private:
  void Listen(const ChannelDef& channel_def);

  void StartMessageLoop(const Status& s);

  void StopMessageLoop();

  void SendMessageLoop();

  void GenerateMessageLoop();

  ClientNode* client_node_;
  Pool<MessageTy, uint8_t> message_queue_;
  std::unique_ptr<Channel<MessageTy>> channel_;
  std::string topic_;
  OnMessageCallback on_message_callback_;

  bool stopped_ = false;

  DISALLOW_COPY_AND_ASSIGN(Publisher);
};

template <typename MessageTy>
void Publisher<MessageTy>::Publish(::base::StringPiece topic,
                                   OnMessageCallback on_message_callback,
                                   const ChannelDef& channel_def) {
  topic_ = std::string(topic);
  on_message_callback_ = on_message_callback;
  message_queue_.reserve(100);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostTask(
      FROM_HERE, ::base::BindOnce(&Publisher<MessageTy>::Listen,
                                  ::base::Unretained(this), channel_def));
}

template <typename MessageTy>
void Publisher<MessageTy>::Listen(const ChannelDef& channel_def) {
  channel_ = ChannelFactory::NewChannel<MessageTy>(channel_def);

  NodeInfo node_info;
  node_info.CopyFrom(client_node_->node_info());
  // Forcely set port 0
  node_info.mutable_ip_endpoint()->set_port(0);

  if (channel_->IsTCPChannel()) {
    TCPChannel<MessageTy>* tcp_channel = channel_->ToTCPChannel();
    tcp_channel->Listen(node_info,
                        ::base::BindOnce(&Publisher<MessageTy>::RequestPublish,
                                         ::base::Unretained(this), channel_def),
                        std::bind(&Publisher<MessageTy>::StartMessageLoop, this,
                                  std::placeholders::_1));
  } else if (channel_->IsUDPChannel()) {
    UDPChannel<MessageTy>* udp_channel = channel_->ToUDPChannel();
    udp_channel->Bind(node_info,
                      ::base::BindOnce(&Publisher<MessageTy>::RequestPublish,
                                       ::base::Unretained(this), channel_def));
  }
}

template <typename MessageTy>
void Publisher<MessageTy>::RequestPublish(
    const ChannelDef& channel_def,
    const StatusOr<::net::IPEndPoint>& status_or) {
  if (status_or.ok()) {
    if (channel_->IsUDPChannel()) {
      StartMessageLoop(Status::OK());
    }

    ::net::IPEndPoint ip_endpoint = status_or.ValueOrDie();
    PublishTopicRequest* request = new PublishTopicRequest();
    TopicSource* topic_source = request->mutable_topic_source();
    topic_source->set_topic(topic_);
    *topic_source->mutable_node_info() = client_node_->node_info();
    IPEndPoint* endpoint = topic_source->mutable_topic_ip_endpoint();
    endpoint->set_ip(ip_endpoint.address().ToString());
    endpoint->set_port(ip_endpoint.port());
    *topic_source->mutable_channel_def() = channel_def;
    PublishTopicResponse* response = new PublishTopicResponse();

    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PublishTopicAsync(
        request, response, [this, request, response](const Status& s) {
          ScopedGrpcRequest<PublishTopicRequest, PublishTopicResponse>
              scoped_request({request, response});
          if (!s.ok()) {
            client_node_->OnError(s);
            StopMessageLoop();
            return;
          }
        });
  } else {
    client_node_->OnError(status_or.status());
  }
}

template <typename MessageTy>
void Publisher<MessageTy>::StartMessageLoop(const Status& s) {
  LOG(INFO) << "Publisher::StartMessageLoop()";
  if (s.ok()) {
    stopped_ = false;
    SendMessageLoop();
    GenerateMessageLoop();
  } else {
    client_node_->OnError(s);
  }
}

template <typename MessageTy>
void Publisher<MessageTy>::StopMessageLoop() {
  channel_.reset();
  stopped_ = true;
}

template <typename MessageTy>
void Publisher<MessageTy>::SendMessageLoop() {
  if (stopped_) return;

  LOG(INFO) << "Publisher::SendMessageLoop()";

  if (!message_queue_.empty()) {
    MessageTy message = std::move(message_queue_.front());
    message_queue_.pop();
    channel_->SendMessage(std::move(message), [this](const Status& s) {
      if (!s.ok()) {
        client_node_->OnError(s);
      }
    });
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostDelayedTask(
      FROM_HERE,
      ::base::BindOnce(&Publisher<MessageTy>::SendMessageLoop,
                       ::base::Unretained(this)),
      ::base::TimeDelta::FromSeconds(1));
}

template <typename MessageTy>
void Publisher<MessageTy>::GenerateMessageLoop() {
  if (stopped_) return;

  LOG(INFO) << "Publisher::GenerateMessageLoop()";

  MessageTy message = on_message_callback_();
  message_queue_.push(std::move(message));

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostDelayedTask(
      FROM_HERE,
      ::base::BindOnce(&Publisher<MessageTy>::GenerateMessageLoop,
                       ::base::Unretained(this)),
      ::base::TimeDelta::FromSeconds(1));
}

}  // namespace felicia

#endif  // FELICIA_CC_COMMUNICATION_PUBLISHER_H_