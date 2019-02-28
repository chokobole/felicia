#ifndef FELICIA_CC_COMMUNICATION_PUBLISHER_H_
#define FELICIA_CC_COMMUNICATION_PUBLISHER_H_

#include <memory>
#include <string>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"
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
  enum State {
    READY,
    STARTED,
    STOPPED,
  };

  struct Settings {
    uint32_t period = 1;
    uint8_t queue_size = 100;
  };

  using OnMessageCallback = ::base::RepeatingCallback<MessageTy(void)>;

  explicit Publisher(ClientNode* client_node) : client_node_(client_node) {}

  void Publish(::base::StringPiece topic, OnMessageCallback on_message_callback,
               const ChannelDef& channel_def = ChannelDef(),
               const Publisher<MessageTy>::Settings& settings = Settings());
  void RequestPublish(const ChannelDef& channel_def,
                      const StatusOr<::net::IPEndPoint>& status_or);

 private:
  void OnPublishTopicAsync(PublishTopicRequest* request,
                           PublishTopicResponse* response, const Status& s);

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

  State state_ = READY;
  Settings settings_;

  DISALLOW_COPY_AND_ASSIGN(Publisher);
};

template <typename MessageTy>
void Publisher<MessageTy>::Publish(
    ::base::StringPiece topic, OnMessageCallback on_message_callback,
    const ChannelDef& channel_def,
    const Publisher<MessageTy>::Settings& settings) {
  topic_ = std::string(topic);
  on_message_callback_ = on_message_callback;
  settings_ = settings;
  message_queue_.reserve(settings_.queue_size);

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
    tcp_channel->Listen(
        node_info,
        ::base::BindOnce(&Publisher<MessageTy>::RequestPublish,
                         ::base::Unretained(this), channel_def),
        ::base::BindRepeating(&Publisher<MessageTy>::StartMessageLoop,
                              ::base::Unretained(this)));
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
        request, response,
        ::base::BindOnce(&Publisher<MessageTy>::OnPublishTopicAsync,
                         ::base::Unretained(this), ::base::Owned(request),
                         ::base::Owned(response)));
  } else {
    client_node_->OnError(status_or.status());
  }
}

template <typename MessageTy>
void Publisher<MessageTy>::OnPublishTopicAsync(PublishTopicRequest* request,
                                               PublishTopicResponse* response,
                                               const Status& s) {
  if (!s.ok()) {
    client_node_->OnError(s);
    StopMessageLoop();
    return;
  }
}

template <typename MessageTy>
void Publisher<MessageTy>::StartMessageLoop(const Status& s) {
  LOG(INFO) << "Publisher::StartMessageLoop()";
  if (s.ok()) {
    if (state_ == STARTED || state_ == STOPPED) return;
    state_ = STARTED;
    SendMessageLoop();
    GenerateMessageLoop();
  } else {
    client_node_->OnError(s);
  }
}

template <typename MessageTy>
void Publisher<MessageTy>::StopMessageLoop() {
  channel_.reset();
  state_ = STOPPED;
}

template <typename MessageTy>
void Publisher<MessageTy>::SendMessageLoop() {
  if (state_ == STOPPED) return;

  LOG(INFO) << "Publisher::SendMessageLoop()";
  if (!message_queue_.empty()) {
    MessageTy message = std::move(message_queue_.front());
    message_queue_.pop();
    if (!channel_->IsSendingMessage()) {
      channel_->SendMessage(
          std::move(message), ::base::BindOnce([](const Status& s) {
            if (!s.ok()) {
              LOG(INFO) << "Failed to send message: " << s.error_message();
            }
          }));
    }
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostDelayedTask(
      FROM_HERE,
      ::base::BindOnce(&Publisher<MessageTy>::SendMessageLoop,
                       ::base::Unretained(this)),
      ::base::TimeDelta::FromSeconds(settings_.period));
}

template <typename MessageTy>
void Publisher<MessageTy>::GenerateMessageLoop() {
  if (state_ == STOPPED) return;

  LOG(INFO) << "Publisher::GenerateMessageLoop()";

  MessageTy message = on_message_callback_.Run();
  message_queue_.push(std::move(message));

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostDelayedTask(
      FROM_HERE,
      ::base::BindOnce(&Publisher<MessageTy>::GenerateMessageLoop,
                       ::base::Unretained(this)),
      ::base::TimeDelta::FromSeconds(settings_.period));
}

}  // namespace felicia

#endif  // FELICIA_CC_COMMUNICATION_PUBLISHER_H_