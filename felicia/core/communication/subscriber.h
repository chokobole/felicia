#ifndef FELICIA_CORE_COMMUNICATION_SUBSCRIBER_H_
#define FELICIA_CORE_COMMUNICATION_SUBSCRIBER_H_

#include <memory>
#include <string>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/compiler_specific.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/communication/register_state.h"
#include "felicia/core/communication/settings.h"
#include "felicia/core/communication/subscriber_state.h"
#include "felicia/core/lib/containers/pool.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/master_proxy.h"

namespace felicia {

template <typename MessageTy>
class Subscriber {
 public:
  using OnMessageCallback = ::base::RepeatingCallback<void(MessageTy&&)>;

  Subscriber() = default;
  virtual ~Subscriber() { DCHECK(IsStopped()) << subscriber_state_.ToString(); }

  ALWAYS_INLINE bool IsRegistering() const {
    return register_state_.IsRegistering();
  }
  ALWAYS_INLINE bool IsRegistered() const {
    return register_state_.IsRegistered();
  }
  ALWAYS_INLINE bool IsUnregistering() const {
    return register_state_.IsUnregistering();
  }
  ALWAYS_INLINE bool IsUnregistered() const {
    return register_state_.IsUnregistered();
  }
  ALWAYS_INLINE bool IsStarted() const { return subscriber_state_.IsStarted(); }
  ALWAYS_INLINE bool IsStopping() const {
    return subscriber_state_.IsStopping();
  }
  ALWAYS_INLINE bool IsStopped() const { return subscriber_state_.IsStopped(); }

  void RequestSubscribe(const NodeInfo& node_info, const std::string& topic,
                        int channel_types,
                        OnMessageCallback on_message_callback,
                        StatusCallback on_error_callback,
                        const communication::Settings& settings,
                        StatusOnceCallback callback);

  void RequestUnsubscribe(const NodeInfo& node_info, const std::string& topic,
                          StatusOnceCallback callback);

 protected:
  void OnSubscribeTopicAsync(SubscribeTopicRequest* request,
                             SubscribeTopicResponse* response,
                             int channel_types,
                             OnMessageCallback on_message_callback,
                             StatusCallback on_error_callback,
                             const communication::Settings& settings,
                             StatusOnceCallback callback, const Status& s);
  void OnUnubscribeTopicAsync(UnsubscribeTopicRequest* request,
                              UnsubscribeTopicResponse* response,
                              StatusOnceCallback callback, const Status& s);

  void OnFindPublisher(const TopicInfo& topic_info);
  void ConnectToPublisher();
  void OnConnectToPublisher(const Status& s);

  void StartMessageLoop();
  void StopMessageLoop(StatusOnceCallback callback);

  void ReceiveMessageLoop();
  void OnReceiveMessage(const Status& s);

  void NotifyMessageLoop();

  void Stop();

  virtual void ResetMessage(const TopicInfo& topic_info) {}

  MessageTy message_;
  Pool<MessageTy, uint8_t> message_queue_;
  TopicInfo topic_info_;
  ::base::Optional<TopicInfo> topic_info_to_update_;
  int channel_types_;
  std::unique_ptr<Channel<MessageTy>> channel_;
  int channel_type_ = 0;
  OnMessageCallback on_message_callback_;
  StatusCallback on_error_callback_;
  StatusOnceCallback on_stopped_callback_;

  communication::RegisterState register_state_;
  communication::SubscriberState subscriber_state_;
  communication::Settings settings_;
  uint8_t receive_message_failed_cnt_ = 0;

  static constexpr uint8_t kMaximumReceiveMessageFailedAllowed = 5;

  DISALLOW_COPY_AND_ASSIGN(Subscriber);
};

template <typename MessageTy>
void Subscriber<MessageTy>::RequestSubscribe(
    const NodeInfo& node_info, const std::string& topic, int channel_types,
    OnMessageCallback on_message_callback, StatusCallback on_error_callback,
    const communication::Settings& settings, StatusOnceCallback callback) {
  if (!IsUnregistered()) {
    std::move(callback).Run(register_state_.InvalidStateError());
    return;
  }

  register_state_.ToRegistering(FROM_HERE);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  SubscribeTopicRequest* request = new SubscribeTopicRequest();
  *request->mutable_node_info() = node_info;
  request->set_topic(topic);
  SubscribeTopicResponse* response = new SubscribeTopicResponse();

  master_proxy.SubscribeTopicAsync(
      request, response,
      ::base::BindOnce(&Subscriber<MessageTy>::OnSubscribeTopicAsync,
                       ::base::Unretained(this), ::base::Owned(request),
                       ::base::Owned(response), channel_types,
                       on_message_callback, on_error_callback, settings,
                       std::move(callback)),
      ::base::BindRepeating(&Subscriber<MessageTy>::OnFindPublisher,
                            ::base::Unretained(this)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::RequestUnsubscribe(const NodeInfo& node_info,
                                               const std::string& topic,
                                               StatusOnceCallback callback) {
  if (!IsRegistered()) {
    std::move(callback).Run(register_state_.InvalidStateError());
    return;
  }

  register_state_.ToUnregistering(FROM_HERE);

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
    int channel_types, OnMessageCallback on_message_callback,
    StatusCallback on_error_callback, const communication::Settings& settings,
    StatusOnceCallback callback, const Status& s) {
  if (!IsRegistering()) {
    std::move(callback).Run(register_state_.InvalidStateError());
    return;
  }

  if (!s.ok()) {
    register_state_.ToUnregistered(FROM_HERE);
    std::move(callback).Run(s);
    return;
  }

  channel_types_ = channel_types;
  on_message_callback_ = on_message_callback;
  on_error_callback_ = on_error_callback;
  settings_ = settings;

  register_state_.ToRegistered(FROM_HERE);
  std::move(callback).Run(s);
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnUnubscribeTopicAsync(
    UnsubscribeTopicRequest* request, UnsubscribeTopicResponse* response,
    StatusOnceCallback callback, const Status& s) {
  if (!IsUnregistering()) {
    std::move(callback).Run(register_state_.InvalidStateError());
    return;
  }

  if (!s.ok()) {
    register_state_.ToRegistered(FROM_HERE);
    std::move(callback).Run(s);
    return;
  }

  register_state_.ToUnregistered(FROM_HERE);
  StopMessageLoop(StatusOnceCallback{});
  std::move(callback).Run(s);
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnFindPublisher(const TopicInfo& topic_info) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  DCHECK(master_proxy.IsBoundToCurrentThread());
  if (IsRegistering() || IsUnregistering() || IsStopping()) {
    master_proxy.PostTask(
        FROM_HERE, ::base::BindOnce(&Subscriber<MessageTy>::OnFindPublisher,
                                    ::base::Unretained(this), topic_info));
    return;
  }

  if (IsUnregistered()) return;

  DCHECK(IsRegistered()) << register_state_.ToString();

  if (topic_info.status() == TopicInfo::UNREGISTERED) {
    if (IsStarted()) {
      StopMessageLoop(StatusOnceCallback{});
    }
    return;
  }

  if (IsStarted()) {
    // UDP Channel can't detect closed connection. So it might be called
    // at |IsStarted()| state. In this case, we have to forcely stop the
    // mesasge loop and try it again.
    DCHECK(channel_->IsUDPChannel());
    topic_info_to_update_ = topic_info;
    return;
  }

  DCHECK(IsStopped()) << subscriber_state_.ToString();

  // If MesageTy is DynamicProtobufMessage, in other words, this class is
  // a instance of DynamicSubscriber, then subscriber resolves its type
  // using |type_name| inside |topic_info|.
  ResetMessage(topic_info);

  // Subscriber holds this in case of data corruption. If it happens, subscriber
  // connects to publisher again using |topic_info_|.
  topic_info_ = topic_info;

  channel_type_ = 1;
  ConnectToPublisher();
}

template <typename MessageTy>
void Subscriber<MessageTy>::ConnectToPublisher() {
  ChannelDef matched_channel_def;
  while (channel_type_ <= channel_types_) {
    if (channel_type_ & channel_types_) {
      for (ChannelDef channel_def : topic_info_.topic_source().channel_defs()) {
        if (channel_def.type() ==
            static_cast<ChannelDef::Type>(channel_type_)) {
          matched_channel_def = channel_def;
          break;
        }
      }
    }
    channel_type_ <<= 1;
  }

  if (matched_channel_def.type() == ChannelDef::CHANNEL_TYPE_NONE) {
    on_error_callback_.Run(
        errors::Unavailable("Failed to connect to publisher."));
    return;
  }

  channel_ = ChannelFactory::NewChannel<MessageTy>(matched_channel_def.type());

  channel_->Connect(
      matched_channel_def,
      ::base::BindOnce(&Subscriber<MessageTy>::OnConnectToPublisher,
                       ::base::Unretained(this)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnConnectToPublisher(const Status& s) {
  if (s.ok()) {
    if (settings_.is_dynamic_buffer) {
      channel_->EnableDynamicBuffer();
    } else {
      channel_->SetReceiveBufferSize(settings_.buffer_size);
    }

    StartMessageLoop();
  } else {
    LOG(ERROR) << "Failed to connect to publisher: " << s;
    channel_type_ <<= 1;
    ConnectToPublisher();
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

  if (IsUnregistering() || IsUnregistered()) return;

  if (IsStopping()) {
    master_proxy.PostDelayedTask(
        FROM_HERE,
        ::base::BindOnce(&Subscriber<MessageTy>::StartMessageLoop,
                         ::base::Unretained(this)),
        settings_.period);
    return;
  }

  if (IsStarted()) return;

  subscriber_state_.ToStarted(FROM_HERE);
  message_queue_.set_capacity(settings_.queue_size);
  ReceiveMessageLoop();
  NotifyMessageLoop();
}

template <typename MessageTy>
void Subscriber<MessageTy>::StopMessageLoop(StatusOnceCallback callback) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE,
        ::base::BindOnce(&Subscriber<MessageTy>::StopMessageLoop,
                         ::base::Unretained(this), std::move(callback)));
    return;
  }

  if (IsStopped() || IsStopping()) {
    LOG(ERROR) << "Tried stopping again";
    std::move(callback).Run(errors::Aborted("Already stopping or stopped."));
    return;
  }

  on_stopped_callback_ = std::move(callback);
  subscriber_state_.ToStopping(FROM_HERE);
}

template <typename MessageTy>
void Subscriber<MessageTy>::ReceiveMessageLoop() {
  if (IsStopping() || IsStopped()) return;

  channel_->ReceiveMessage(
      &message_, ::base::BindOnce(&Subscriber<MessageTy>::OnReceiveMessage,
                                  ::base::Unretained(this)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnReceiveMessage(const Status& s) {
  if (IsStopping() || IsStopped()) return;

  if (s.ok()) {
    receive_message_failed_cnt_ = 0;
    message_queue_.push(message_);
  } else {
    Status new_status(s.error_code(),
                      ::base::StringPrintf("Failed to receive a message: %s",
                                           s.error_message().c_str()));
    on_error_callback_.Run(new_status);
    if (channel_->IsTCPChannel() && !channel_->ToTCPChannel()->IsConnected()) {
      StopMessageLoop(StatusOnceCallback{});
      return;
    }
    receive_message_failed_cnt_++;
    if (receive_message_failed_cnt_ >= kMaximumReceiveMessageFailedAllowed) {
      StopMessageLoop(StatusOnceCallback{});
      return;
    }
  }

  if (channel_->IsShmChannel()) {
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostDelayedTask(
        FROM_HERE,
        ::base::BindOnce(&Subscriber<MessageTy>::ReceiveMessageLoop,
                         ::base::Unretained(this)),
        settings_.period);
  } else {
    ReceiveMessageLoop();
  }
}

template <typename MessageTy>
void Subscriber<MessageTy>::NotifyMessageLoop() {
  if (IsStopped()) return;

  if (!message_queue_.empty()) {
    MessageTy message = std::move(message_queue_.front());
    message_queue_.pop();
    on_message_callback_.Run(std::move(message));
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (IsStopping() && message_queue_.empty()) {
    master_proxy.PostDelayedTask(
        FROM_HERE,
        ::base::BindOnce(&Subscriber<MessageTy>::Stop,
                         ::base::Unretained(this)),
        settings_.period + ::base::TimeDelta::FromMilliseconds(
                               100));  // Add some offset for safe close.
  } else {
    master_proxy.PostDelayedTask(
        FROM_HERE,
        ::base::BindOnce(&Subscriber<MessageTy>::NotifyMessageLoop,
                         ::base::Unretained(this)),
        settings_.period);
  }
}

// Should carefully release the resources.
// |channel_| should be released on MasterProxy thread,
// if you release |message_queue_|, |on_message_callback_| and
// |on_error_callback_| too early, then it might be crashed on
// |NotifyMessageLoop|, which loops every |settings_.period|.
template <typename MessageTy>
void Subscriber<MessageTy>::Stop() {
#if DCHECK_IS_ON()
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  DCHECK(master_proxy.IsBoundToCurrentThread());
#endif

  if (IsUnregistered()) {
    channel_.reset();
    message_queue_.clear();

    on_message_callback_.Reset();
    on_error_callback_.Reset();
  }

  subscriber_state_.ToStopped(FROM_HERE);

  // Stop called route
  // 1. User's manual unsubscribe
  // 2. Message corruption
  // 3. Master's notification that publisher is off.
  // 4. Master's notification that topic info is updated.

  if (on_stopped_callback_.is_null()) {
    // If |on_stopped_callback_| is not null, then we have to expect this
    // subscriber can be destroied from the caller waiting callback side.
    // So even if there is |topic_info_to_update_|, it's better to ignore.
    // For the same reason,
    if (topic_info_to_update_.has_value()) {
      TopicInfo topic_info = topic_info_to_update_.value();
      topic_info_to_update_.reset();
      OnFindPublisher(topic_info);
    } else if (receive_message_failed_cnt_ >=
               kMaximumReceiveMessageFailedAllowed) {
      channel_type_ = 1;
      ConnectToPublisher();
    }
  } else {
    std::move(on_stopped_callback_).Run(Status::OK());
  }
}

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_SUBSCRIBER_H_