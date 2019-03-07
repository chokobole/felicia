#ifndef FELICIA_CC_COMMUNICATION_SUBSCRIBER_H_
#define FELICIA_CC_COMMUNICATION_SUBSCRIBER_H_

#include <memory>
#include <string>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/cc/master_proxy.h"
#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/containers/pool.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

template <typename MessageTy>
class EXPORT Subscriber {
 public:
  enum State {
    READY,
    STARTED,
    STOPPED,
  };

  struct Settings {
    uint32_t period = 1;  // in seconds
    uint8_t queue_size = 100;
  };

  using OnMessageCallback = ::base::RepeatingCallback<void(const MessageTy&)>;

  explicit Subscriber(NodeLifecycle* node_lifecycle)
      : node_lifecycle_(node_lifecycle) {}

  void set_node_info(const NodeInfo& node_info) { node_info_ = node_info; }

  bool IsRunning() const { return state_ == STARTED; }
  bool IsStopped() const { return state_ == STOPPED; }

  void Subscribe(const std::string& topic,
                 OnMessageCallback on_message_callback,
                 const Subscriber<MessageTy>::Settings& settings = Settings());

 private:
  void OnSubscribeTopicAsync(SubscribeTopicRequest* request,
                             SubscribeTopicResponse* response, const Status& s);

  void OnFindPublisher(const TopicInfo& topic_info);
  void OnConnectToPublisher(const Status& s);

  void StartMessageLoop();
  void StopMessageLoop();

  void ReceiveMessageLoop();
  void OnReceiveMessage(const Status& s);

  void NotifyMessageLoop();

  NodeLifecycle* node_lifecycle_;  // not owned
  NodeInfo node_info_;
  MessageTy message_;
  Pool<MessageTy, uint8_t> message_queue_;
  std::unique_ptr<Channel<MessageTy>> channel_;
  OnMessageCallback on_message_callback_;

  State state_;
  ::base::TimeDelta period_;

  DISALLOW_COPY_AND_ASSIGN(Subscriber);
};

template <typename MessageTy>
void Subscriber<MessageTy>::Subscribe(
    const std::string& topic, OnMessageCallback on_message_callback,
    const Subscriber<MessageTy>::Settings& settings) {
  on_message_callback_ = on_message_callback;
  message_queue_.reserve(settings.queue_size);
  period_ = ::base::TimeDelta::FromSeconds(settings.period);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  SubscribeTopicRequest* request = new SubscribeTopicRequest();
  *request->mutable_node_info() = node_info_;
  request->set_topic(topic);
  SubscribeTopicResponse* response = new SubscribeTopicResponse();

  master_proxy.SubscribeTopicAsync(
      request, response,
      ::base::BindOnce(&Subscriber<MessageTy>::OnSubscribeTopicAsync,
                       ::base::Unretained(this), ::base::Owned(request),
                       ::base::Owned(response)),
      ::base::BindRepeating(&Subscriber<MessageTy>::OnFindPublisher,
                            ::base::Unretained(this)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnSubscribeTopicAsync(
    SubscribeTopicRequest* request, SubscribeTopicResponse* response,
    const Status& s) {
  if (!s.ok()) {
    Status new_status = Status(
        s.error_code(),
        ::base::StringPrintf("Failed to register to subsccribe topic: %s",
                             s.error_message().c_str()));
    node_lifecycle_->OnError(new_status);
    return;
  }
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnFindPublisher(const TopicInfo& topic_info) {
  channel_ = ChannelFactory::NewChannel<MessageTy>(
      topic_info.topic_source().channel_def());

  channel_->Connect(
      topic_info.topic_source(),
      ::base::BindOnce(&Subscriber<MessageTy>::OnConnectToPublisher,
                       ::base::Unretained(this)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnConnectToPublisher(const Status& s) {
  if (s.ok()) {
    StartMessageLoop();
  } else {
    Status new_status(s.error_code(),
                      ::base::StringPrintf("Failed to connect to publisher: %s",
                                           s.error_message().c_str()));
    node_lifecycle_->OnError(new_status);
  }
}

template <typename MessageTy>
void Subscriber<MessageTy>::StartMessageLoop() {
  if (state_ == STARTED) return;
  state_ = STARTED;
  ReceiveMessageLoop();
  NotifyMessageLoop();
}

template <typename MessageTy>
void Subscriber<MessageTy>::StopMessageLoop() {
  if (state_ == STOPPED) return;
  state_ = STOPPED;
}

template <typename MessageTy>
void Subscriber<MessageTy>::ReceiveMessageLoop() {
  if (state_ == STOPPED) return;

  channel_->ReceiveMessage(
      &message_, ::base::BindOnce(&Subscriber<MessageTy>::OnReceiveMessage,
                                  ::base::Unretained(this)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnReceiveMessage(const Status& s) {
  if (state_ == STOPPED) return;

  if (s.ok()) {
    message_queue_.push(std::move(message_));
  } else {
    Status new_status(s.error_code(),
                      ::base::StringPrintf("Failed to receive a message: %s",
                                           s.error_message().c_str()));
    node_lifecycle_->OnError(new_status);
    if (channel_->IsTCPChannel() && !channel_->ToTCPChannel()->IsConnected()) {
      StopMessageLoop();
      return;
    }
  }
  ReceiveMessageLoop();
}

template <typename MessageTy>
void Subscriber<MessageTy>::NotifyMessageLoop() {
  if (state_ == STOPPED && message_queue_.empty()) return;

  if (!message_queue_.empty()) {
    MessageTy message = std::move(message_queue_.front());
    message_queue_.pop();
    on_message_callback_.Run(message);
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostDelayedTask(
      FROM_HERE,
      ::base::BindOnce(&Subscriber<MessageTy>::NotifyMessageLoop,
                       ::base::Unretained(this)),
      period_);
}

}  // namespace felicia

#endif  // FELICIA_CC_COMMUNICATION_SUBSCRIBER_H_