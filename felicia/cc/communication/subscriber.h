#ifndef FELICIA_CC_COMMUNICATION_SUBSCRIBER_H_
#define FELICIA_CC_COMMUNICATION_SUBSCRIBER_H_

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
class EXPORT Subscriber {
 public:
  using OnMessageCallback = std::function<void(MessageTy)>;

  explicit Subscriber(ClientNode* client_node) : client_node_(client_node) {}

  void Subscribe(::base::StringPiece topic,
                 OnMessageCallback on_message_callback);

 private:
  void OnFindPublisher(const TopicSource& source);

  void StopMessageLoop();

  void ReceiveMessageLoop();
  void OnReceiveMessage(const Status& s);

  void NotifyMessageLoop();

  ClientNode* client_node_;
  MessageTy message_;
  Pool<MessageTy, uint8_t> message_queue_;
  std::unique_ptr<Channel<MessageTy>> channel_;
  std::string topic_;
  OnMessageCallback on_message_callback_;

  bool stopped_ = false;

  DISALLOW_COPY_AND_ASSIGN(Subscriber);
};

template <typename MessageTy>
void Subscriber<MessageTy>::Subscribe(::base::StringPiece topic,
                                      OnMessageCallback on_message_callback) {
  topic_ = std::string(topic);
  on_message_callback_ = on_message_callback;
  message_queue_.reserve(100);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  SubscribeTopicRequest* request = new SubscribeTopicRequest();
  *request->mutable_node_info() = client_node_->node_info();
  request->set_topic(topic_);
  SubscribeTopicResponse* response = new SubscribeTopicResponse();

  master_proxy.SubscribeTopicAsync(
      request, response,
      [this, request, response](const Status& s) {
        ScopedGrpcRequest<SubscribeTopicRequest, SubscribeTopicResponse>
            scoped_request({request, response});
        if (!s.ok()) {
          client_node_->OnError(s);
          StopMessageLoop();
          return;
        }
        NotifyMessageLoop();
      },
      ::base::BindRepeating(&Subscriber<MessageTy>::OnFindPublisher,
                            ::base::Unretained(this)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnFindPublisher(const TopicSource& source) {
  channel_ = ChannelFactory::NewChannel<MessageTy>(source.channel_def());

  channel_->Connect(source, [this](const Status& s) {
    if (s.ok()) {
      ReceiveMessageLoop();
    } else {
      client_node_->OnError(s);
    }
  });
}

template <typename MessageTy>
void Subscriber<MessageTy>::StopMessageLoop() {
  channel_.reset();
  stopped_ = true;
}

template <typename MessageTy>
void Subscriber<MessageTy>::ReceiveMessageLoop() {
  if (stopped_) return;

  LOG(INFO) << "Subscriber::ReceiveMessageLoop()";
  channel_->ReceiveMessage(
      &message_, std::bind(&Subscriber<MessageTy>::OnReceiveMessage, this,
                           std::placeholders::_1));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnReceiveMessage(const Status& s) {
  if (stopped_) return;

  if (s.ok()) {
    message_queue_.push(std::move(message_));
  } else {
    client_node_->OnError(s);
  }
  ReceiveMessageLoop();
}

template <typename MessageTy>
void Subscriber<MessageTy>::NotifyMessageLoop() {
  if (stopped_) return;

  if (!message_queue_.empty()) {
    MessageTy message = std::move(message_queue_.front());
    message_queue_.pop();
    on_message_callback_(std::move(message));
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostDelayedTask(
      FROM_HERE,
      ::base::BindOnce(&Subscriber<MessageTy>::NotifyMessageLoop,
                       ::base::Unretained(this)),
      ::base::TimeDelta::FromSeconds(1));
}

}  // namespace felicia

#endif  // FELICIA_CC_COMMUNICATION_SUBSCRIBER_H_