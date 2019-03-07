#ifndef FELICIA_CC_COMMUNICATION_PUBLISHER_H_
#define FELICIA_CC_COMMUNICATION_PUBLISHER_H_

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
class EXPORT Publisher {
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

  using OnMessageCallback = ::base::RepeatingCallback<MessageTy(void)>;

  explicit Publisher(NodeLifecycle* node_lifecycle)
      : node_lifecycle_(node_lifecycle) {}

  void set_node_info(const NodeInfo& node_info) { node_info_ = node_info; }

  bool IsRunning() const { return state_ == STARTED; }
  bool IsStopped() const { return state_ == STOPPED; }

  void Publish(const std::string& topic, OnMessageCallback on_message_callback,
               const ChannelDef& channel_def,
               const Publisher<MessageTy>::Settings& settings = Settings());

 private:
  void OnPublishTopicAsync(PublishTopicRequest* request,
                           PublishTopicResponse* response, const Status& s);

  void Setup(const std::string& topic, const ChannelDef& channel_def);

  void StartMessageLoop(const Status& s);

  void StopMessageLoop();

  void SendMessageLoop();
  void OnSendMessageLoop(const Status& s);

  void GenerateMessageLoop();

  NodeLifecycle* node_lifecycle_;  // not owned
  NodeInfo node_info_;
  Pool<MessageTy, uint8_t> message_queue_;
  std::unique_ptr<Channel<MessageTy>> channel_;
  OnMessageCallback on_message_callback_;

  State state_ = READY;
  ::base::TimeDelta period_;

  DISALLOW_COPY_AND_ASSIGN(Publisher);
};

template <typename MessageTy>
void Publisher<MessageTy>::Publish(
    const std::string& topic, OnMessageCallback on_message_callback,
    const ChannelDef& channel_def,
    const Publisher<MessageTy>::Settings& settings) {
  on_message_callback_ = on_message_callback;
  message_queue_.reserve(settings.queue_size);
  period_ = ::base::TimeDelta::FromSeconds(settings.period);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostTask(
      FROM_HERE,
      ::base::BindOnce(&Publisher<MessageTy>::Setup, ::base::Unretained(this),
                       topic, channel_def));
}

template <typename MessageTy>
void Publisher<MessageTy>::Setup(const std::string& topic,
                                 const ChannelDef& channel_def) {
  channel_ = ChannelFactory::NewChannel<MessageTy>(channel_def);

  StatusOr<ChannelSource> status_or;
  if (channel_->IsTCPChannel()) {
    TCPChannel<MessageTy>* tcp_channel = channel_->ToTCPChannel();
    status_or = tcp_channel->Listen();
  } else if (channel_->IsUDPChannel()) {
    UDPChannel<MessageTy>* udp_channel = channel_->ToUDPChannel();
    status_or = udp_channel->Bind();
  }

  if (!status_or.ok()) {
    Status new_status = Status(
        status_or.status().error_code(),
        ::base::StringPrintf("Failed to set up publisher: %s",
                             status_or.status().error_message().c_str()));
    node_lifecycle_->OnError(new_status);
    return;
  }

  PublishTopicRequest* request = new PublishTopicRequest();
  *request->mutable_node_info() = node_info_;
  TopicInfo* topic_info = request->mutable_topic_info();
  topic_info->set_topic(topic);
  *topic_info->mutable_topic_source() = status_or.ValueOrDie();
  PublishTopicResponse* response = new PublishTopicResponse();

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PublishTopicAsync(
      request, response,
      ::base::BindOnce(&Publisher<MessageTy>::OnPublishTopicAsync,
                       ::base::Unretained(this), ::base::Owned(request),
                       ::base::Owned(response)));

  if (channel_->IsTCPChannel()) {
    channel_->ToTCPChannel()->DoAcceptLoop(::base::BindRepeating(
        &Publisher<MessageTy>::StartMessageLoop, ::base::Unretained(this)));
  }
}

template <typename MessageTy>
void Publisher<MessageTy>::OnPublishTopicAsync(PublishTopicRequest* request,
                                               PublishTopicResponse* response,
                                               const Status& s) {
  if (!s.ok()) {
    Status new_status =
        Status(s.error_code(),
               ::base::StringPrintf("Failed to register to publish topic: %s",
                                    s.error_message().c_str()));
    node_lifecycle_->OnError(new_status);
    return;
  }
  if (channel_->IsUDPChannel()) {
    StartMessageLoop(Status::OK());
  }
}

template <typename MessageTy>
void Publisher<MessageTy>::StartMessageLoop(const Status& s) {
  if (s.ok()) {
    if (state_ == STARTED) return;
    state_ = STARTED;
    SendMessageLoop();
    GenerateMessageLoop();
  } else {
    Status new_status(s.error_code(),
                      ::base::StringPrintf("Failed to start message loop: %s",
                                           s.error_message().c_str()));
    node_lifecycle_->OnError(new_status);
  }
}

template <typename MessageTy>
void Publisher<MessageTy>::StopMessageLoop() {
  if (state_ == STOPPED) return;
  state_ = STOPPED;
}

template <typename MessageTy>
void Publisher<MessageTy>::SendMessageLoop() {
  if (state_ == STOPPED) return;

  if (!message_queue_.empty()) {
    MessageTy message = std::move(message_queue_.front());
    message_queue_.pop();
    if (!channel_->IsSendingMessage()) {
      channel_->SendMessage(
          message, ::base::BindOnce(&Publisher<MessageTy>::OnSendMessageLoop,
                                    ::base::Unretained(this)));
    }
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostDelayedTask(
      FROM_HERE,
      ::base::BindOnce(&Publisher<MessageTy>::SendMessageLoop,
                       ::base::Unretained(this)),
      period_);
}

template <typename MessageTy>
void Publisher<MessageTy>::OnSendMessageLoop(const Status& s) {
  if (!s.ok()) {
    Status new_status(s.error_code(),
                      ::base::StringPrintf("Failed to send a message: %s",
                                           s.error_message().c_str()));
    node_lifecycle_->OnError(new_status);
    if (channel_->IsTCPChannel() && !channel_->ToTCPChannel()->IsConnected()) {
      StopMessageLoop();
      return;
    }
  }
}

template <typename MessageTy>
void Publisher<MessageTy>::GenerateMessageLoop() {
  if (state_ == STOPPED) {
    while (!message_queue_.empty()) message_queue_.pop();
    return;
  }

  MessageTy message = on_message_callback_.Run();
  message_queue_.push(std::move(message));

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostDelayedTask(
      FROM_HERE,
      ::base::BindOnce(&Publisher<MessageTy>::GenerateMessageLoop,
                       ::base::Unretained(this)),
      period_);
}

}  // namespace felicia

#endif  // FELICIA_CC_COMMUNICATION_PUBLISHER_H_