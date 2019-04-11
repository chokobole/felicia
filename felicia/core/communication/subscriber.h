#ifndef FELICIA_CC_COMMUNICATION_SUBSCRIBER_H_
#define FELICIA_CC_COMMUNICATION_SUBSCRIBER_H_

#include <memory>
#include <string>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/compiler_specific.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/communication/state.h"
#include "felicia/core/lib/containers/pool.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/master_proxy.h"

namespace felicia {

namespace communication {

struct Settings {
  Settings(uint32_t period = 1000, uint8_t queue_size = 100)
      : period(period), queue_size(queue_size) {}

  uint32_t period;  // in milliseconds
  uint8_t queue_size;
};

}  // namespace communication

template <typename MessageTy>
class Subscriber {
 public:
  using OnMessageCallback = ::base::RepeatingCallback<void(const MessageTy&)>;
  using OnErrorCallback = ::base::RepeatingCallback<void(const Status& s)>;

  Subscriber() { state_.ToUneregistered(); }
  ~Subscriber() { DCHECK(IsStopped()); }

  ALWAYS_INLINE bool IsRegistering() const { return state_.IsRegistering(); }
  ALWAYS_INLINE bool IsRegistered() const { return state_.IsRegistered(); }
  ALWAYS_INLINE bool IsUnregistering() const {
    return state_.IsUnregistering();
  }
  ALWAYS_INLINE bool IsUnregistered() const { return state_.IsUnregistered(); }
  ALWAYS_INLINE bool IsStarted() const { return state_.IsStarted(); }
  ALWAYS_INLINE bool IsStopped() const { return state_.IsStopped(); }

  void RequestSubscribe(const NodeInfo& node_info, const std::string& topic,
                        OnMessageCallback on_message_callback,
                        OnErrorCallback on_error_callback,
                        const communication::Settings& settings,
                        StatusCallback callback);

  void RequestUnsubscribe(const NodeInfo& node_info, const std::string& topic,
                          StatusCallback callback);

 protected:
  void OnSubscribeTopicAsync(SubscribeTopicRequest* request,
                             SubscribeTopicResponse* response,
                             StatusCallback callback,
                             OnMessageCallback on_message_callback,
                             const communication::Settings& settings,
                             const Status& s);
  void OnUnubscribeTopicAsync(UnsubscribeTopicRequest* request,
                              UnsubscribeTopicResponse* response,
                              StatusCallback callback, const Status& s);

  void OnFindPublisher(const TopicInfo& topic_info);
  void OnConnectToPublisher(const Status& s);

  void StartMessageLoop();
  void StopMessageLoop();

  void ReceiveMessageLoop();
  void OnReceiveMessage(const Status& s);

  void NotifyMessageLoop();

  void Release();

  virtual void ResetMessage(const TopicInfo& topic_info) {}

  MessageTy message_;
  Pool<MessageTy, uint8_t> message_queue_;
  std::unique_ptr<Channel<MessageTy>> channel_;
  OnMessageCallback on_message_callback_;
  OnErrorCallback on_error_callback_;

  communication::State last_state_;
  communication::State state_;
  uint8_t queue_size_;
  ::base::TimeDelta period_;

  DISALLOW_COPY_AND_ASSIGN(Subscriber);
};

template <typename MessageTy>
void Subscriber<MessageTy>::RequestSubscribe(
    const NodeInfo& node_info, const std::string& topic,
    OnMessageCallback on_message_callback, OnErrorCallback on_error_callback,
    const communication::Settings& settings, StatusCallback callback) {
  if (!(IsUnregistered() || IsStopped())) {
    std::move(callback).Run(state_.InvalidStateError());
    return;
  }

  on_error_callback_ = on_error_callback;

  last_state_ = state_;
  state_.ToRegistering();

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  SubscribeTopicRequest* request = new SubscribeTopicRequest();
  *request->mutable_node_info() = node_info;
  request->set_topic(topic);
  SubscribeTopicResponse* response = new SubscribeTopicResponse();

  master_proxy.SubscribeTopicAsync(
      request, response,
      ::base::BindOnce(&Subscriber<MessageTy>::OnSubscribeTopicAsync,
                       ::base::Unretained(this), ::base::Owned(request),
                       ::base::Owned(response), std::move(callback),
                       on_message_callback, settings),
      ::base::BindRepeating(&Subscriber<MessageTy>::OnFindPublisher,
                            ::base::Unretained(this)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::RequestUnsubscribe(const NodeInfo& node_info,
                                               const std::string& topic,
                                               StatusCallback callback) {
  if (!(IsRegistered() || IsStarted())) {
    std::move(callback).Run(state_.InvalidStateError());
    return;
  }

  last_state_ = state_;
  state_.ToUnregistering();

  MasterProxy& master_proxy = MasterProxy::GetInstance();

  UnsubscribeTopicRequest* request = new UnsubscribeTopicRequest();
  *request->mutable_node_info() = node_info;
  request->set_topic(topic);
  UnsubscribeTopicResponse* response = new UnsubscribeTopicResponse();

  master_proxy.UnsubscribeTopicAsync(
      request, response,
      ::base::BindOnce(&Subscriber<MessageTy>::OnUnubscribeTopicAsync,
                       ::base::Unretained(this), ::base::Owned(request),
                       ::base::Owned(response), std::move(callback)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnSubscribeTopicAsync(
    SubscribeTopicRequest* request, SubscribeTopicResponse* response,
    StatusCallback callback, OnMessageCallback on_message_callback,
    const communication::Settings& settings, const Status& s) {
  DCHECK(IsRegistering());

  if (!s.ok()) {
    state_ = last_state_;
    last_state_.ToUnknown();
    std::move(callback).Run(s);
    return;
  }

  on_message_callback_ = on_message_callback;
  queue_size_ = settings.queue_size;
  period_ = ::base::TimeDelta::FromMilliseconds(settings.period);

  state_.ToRegistered();
  std::move(callback).Run(s);
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnUnubscribeTopicAsync(
    UnsubscribeTopicRequest* request, UnsubscribeTopicResponse* response,
    StatusCallback callback, const Status& s) {
  DCHECK(IsUnregistering());

  if (!s.ok()) {
    state_ = last_state_;
    last_state_.ToUnknown();
    std::move(callback).Run(s);
    return;
  }

  state_.ToUneregistered();
  StopMessageLoop();
  std::move(callback).Run(s);
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnFindPublisher(const TopicInfo& topic_info) {
#if DCHECK_IS_ON()
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  DCHECK(master_proxy.IsBoundToCurrentThread());
#endif
  if (IsStarted()) {
    // UDP Channel can't detect closed connection. So it might be called
    // at |IsStarted()| state. In this case, we have to forcely stop the
    // mesasge loop and try it again.
    DCHECK(channel_->IsUDPChannel());
    StopMessageLoop();
#if !DCHECK_IS_ON()
    MasterProxy& master_proxy = MasterProxy::GetInstance();
#endif
    master_proxy.PostTask(
        FROM_HERE, ::base::BindOnce(&Subscriber<MessageTy>::OnFindPublisher,
                                    ::base::Unretained(this), topic_info));
    return;
  }

  ResetMessage(topic_info);

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
    on_error_callback_.Run(new_status);
  }
}

template <typename MessageTy>
void Subscriber<MessageTy>::StartMessageLoop() {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE, ::base::BindOnce(&Subscriber<MessageTy>::StartMessageLoop,
                                    ::base::Unretained(this)));
    return;
  }

  if (IsStarted()) return;

  if (!(IsRegistered() || IsStopped())) {
    Status s = state_.InvalidStateError();
    on_error_callback_.Run(Status{
        s.error_code(), ::base::StringPrintf("Failed to start mesasge loop: %s",
                                             s.error_message().c_str())});
    return;
  }

  state_.ToStarted();
  message_queue_.set_capacity(queue_size_);
  ReceiveMessageLoop();
  NotifyMessageLoop();
}

template <typename MessageTy>
void Subscriber<MessageTy>::StopMessageLoop() {
  if (IsStopped()) return;

  if (!(IsUnregistered() || IsStarted())) {
    Status s = state_.InvalidStateError();
    Status new_status = Status{
        s.error_code(), ::base::StringPrintf("Failed to stop mesasge loop: %s",
                                             s.error_message().c_str())};
    on_error_callback_.Run(new_status);
    return;
  }

  Release();
  state_.ToStopped();
}

template <typename MessageTy>
void Subscriber<MessageTy>::ReceiveMessageLoop() {
  if (IsStopped()) return;

  channel_->ReceiveMessage(
      &message_, ::base::BindOnce(&Subscriber<MessageTy>::OnReceiveMessage,
                                  ::base::Unretained(this)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnReceiveMessage(const Status& s) {
  if (IsStopped()) return;

  if (s.ok()) {
    message_queue_.push(std::move(message_));
  } else {
    Status new_status(s.error_code(),
                      ::base::StringPrintf("Failed to receive a message: %s",
                                           s.error_message().c_str()));
    on_error_callback_.Run(new_status);
    if (channel_->IsTCPChannel() && !channel_->ToTCPChannel()->IsConnected()) {
      StopMessageLoop();
      return;
    }
  }
  ReceiveMessageLoop();
}

template <typename MessageTy>
void Subscriber<MessageTy>::NotifyMessageLoop() {
  if (IsStopped()) return;

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

template <typename MessageTy>
void Subscriber<MessageTy>::Release() {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(FROM_HERE,
                          ::base::BindOnce(&Subscriber<MessageTy>::Release,
                                           ::base::Unretained(this)));
    return;
  }

  channel_.reset();
  message_queue_.clear();
}

}  // namespace felicia

#endif  // FELICIA_CC_COMMUNICATION_SUBSCRIBER_H_