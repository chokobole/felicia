// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
#include "felicia/core/channel/message_receiver.h"
#include "felicia/core/channel/ros_topic_request.h"
#include "felicia/core/communication/register_state.h"
#include "felicia/core/communication/settings.h"
#include "felicia/core/communication/subscriber_state.h"
#include "felicia/core/lib/containers/pool.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/core/message/ros_protocol.h"
#include "felicia/core/thread/main_thread.h"

namespace felicia {

template <typename MessageTy>
class Subscriber {
 public:
  using OnMessageCallback = base::RepeatingCallback<void(MessageTy&&)>;

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
                        const communication::Settings& settings,
                        OnMessageCallback on_message_callback,
                        StatusCallback on_error_callback = StatusCallback(),
                        StatusOnceCallback callback = StatusOnceCallback());

  void RequestUnsubscribe(const NodeInfo& node_info, const std::string& topic,
                          StatusOnceCallback callback = StatusOnceCallback());

 private:
  friend class PubSubTest;

  void RequestSubscribeForTesting(
      const std::string& topic, int channel_types,
      const communication::Settings& settings,
      OnMessageCallback on_message_callback,
      StatusCallback on_error_callback = StatusCallback());
  void RequestUnsubscribeForTesting(const std::string& topic);

 protected:
  friend class RosTopicRequest;

  void OnSubscribeTopicAsync(const SubscribeTopicRequest* request,
                             const SubscribeTopicResponse* response,
                             int channel_types,
                             const communication::Settings& settings,
                             OnMessageCallback on_message_callback,
                             StatusCallback on_error_callback,
                             StatusOnceCallback callback, Status s);
  void OnUnubscribeTopicAsync(const UnsubscribeTopicRequest* request,
                              const UnsubscribeTopicResponse* response,
                              StatusOnceCallback callback, Status s);

  void OnFindPublisher(const TopicInfo& topic_info);
  void ConnectToPublisher();
  void OnConnectToPublisher(Status s);
#if defined(HAS_ROS)
  void OnRosTopicHandshake(Status s);
#endif  // defined(HAS_ROS)

  void StartMessageLoop();
  void StopMessageLoop(StatusOnceCallback callback = StatusOnceCallback());

  void ReceiveMessageLoop();
  void OnReceiveMessage(Status s);

  void NotifyMessageLoop();

  void Stop();

  // SerializedMessageSubscirber must override GetMessageMD5Sum and
  // GetMesasgeTypeName methods.
#if defined(HAS_ROS)
  virtual std::string GetMessageMD5Sum() const {
    return MessageIO<MessageTy>::MD5Sum();
  }
#endif  // defined(HAS_ROS)

  // Because type of DynamicProtobufMessage or SerializedMessage can't be
  // determined at compile time. We should workaround by doing runtime
  // asking its Publisher.
  virtual std::string GetMessageTypeName() const {
    return MessageIO<MessageTy>::TypeName();
  }

  // Needed by DynamicSubscriber, because it cann't resolve its message type
  // until master notification is reached.
  virtual bool MaybeResolveMessgaeType(const TopicInfo& topic_info) {
    return true;
  }

  Pool<MessageTy, uint8_t> message_queue_;
  TopicInfo topic_info_;
  base::Optional<TopicInfo> topic_info_to_update_;
  int channel_types_;
  std::unique_ptr<Channel> channel_;
#if defined(HAS_ROS)
  RosTopicRequest topic_request_;
#endif  // defined(HAS_ROS)
  MessageReceiver<MessageTy> message_receiver_;

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
    const communication::Settings& settings,
    OnMessageCallback on_message_callback, StatusCallback on_error_callback,
    StatusOnceCallback callback) {
  MainThread& main_thread = MainThread::GetInstance();
  if (!main_thread.IsBoundToCurrentThread()) {
    main_thread.PostTask(
        FROM_HERE, base::BindOnce(&Subscriber<MessageTy>::RequestSubscribe,
                                  base::Unretained(this), node_info, topic,
                                  channel_types, settings, on_message_callback,
                                  on_error_callback, std::move(callback)));
    return;
  }

  if (!IsUnregistered()) {
    internal::LogOrCallback(std::move(callback),
                            register_state_.InvalidStateError());
    return;
  }

  register_state_.ToRegistering(FROM_HERE);

  SubscribeTopicRequest* request = new SubscribeTopicRequest();
  *request->mutable_node_info() = node_info;
  request->set_topic(topic);
  request->set_topic_type(GetMessageTypeName());
  SubscribeTopicResponse* response = new SubscribeTopicResponse();

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.SubscribeTopicAsync(
      request, response,
      base::BindOnce(
          &Subscriber<MessageTy>::OnSubscribeTopicAsync, base::Unretained(this),
          base::Owned(request), base::Owned(response), channel_types, settings,
          on_message_callback, on_error_callback, std::move(callback)),
      base::BindRepeating(&Subscriber<MessageTy>::OnFindPublisher,
                          base::Unretained(this)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::RequestUnsubscribe(const NodeInfo& node_info,
                                               const std::string& topic,
                                               StatusOnceCallback callback) {
  MainThread& main_thread = MainThread::GetInstance();
  if (!main_thread.IsBoundToCurrentThread()) {
    main_thread.PostTask(
        FROM_HERE, base::BindOnce(&Subscriber<MessageTy>::RequestUnsubscribe,
                                  base::Unretained(this), node_info, topic,
                                  std::move(callback)));
    return;
  }

  if (!IsRegistered()) {
    internal::LogOrCallback(std::move(callback),
                            register_state_.InvalidStateError());
    return;
  }

  register_state_.ToUnregistering(FROM_HERE);

  UnsubscribeTopicRequest* request = new UnsubscribeTopicRequest();
  *request->mutable_node_info() = node_info;
  request->set_topic(topic);
  UnsubscribeTopicResponse* response = new UnsubscribeTopicResponse();

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.UnsubscribeTopicAsync(
      request, response,
      base::BindOnce(&Subscriber<MessageTy>::OnUnubscribeTopicAsync,
                     base::Unretained(this), base::Owned(request),
                     base::Owned(response), std::move(callback)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::RequestSubscribeForTesting(
    const std::string& topic, int channel_types,
    const communication::Settings& settings,
    OnMessageCallback on_message_callback, StatusCallback on_error_callback) {
  MainThread& main_thread = MainThread::GetInstance();
  if (!main_thread.IsBoundToCurrentThread()) {
    main_thread.PostTask(
        FROM_HERE,
        base::BindOnce(&Subscriber<MessageTy>::RequestSubscribeForTesting,
                       base::Unretained(this), topic, channel_types, settings,
                       on_message_callback, on_error_callback));
    return;
  }

  if (!IsUnregistered()) {
    LOG(ERROR) << register_state_.InvalidStateError();
    return;
  }

  register_state_.ToRegistering(FROM_HERE);

  OnSubscribeTopicAsync(nullptr, nullptr, channel_types, settings,
                        on_message_callback, on_error_callback,
                        StatusOnceCallback(), Status::OK());
}

template <typename MessageTy>
void Subscriber<MessageTy>::RequestUnsubscribeForTesting(
    const std::string& topic) {
  MainThread& main_thread = MainThread::GetInstance();
  if (!main_thread.IsBoundToCurrentThread()) {
    main_thread.PostTask(
        FROM_HERE,
        base::BindOnce(&Subscriber<MessageTy>::RequestUnsubscribeForTesting,
                       base::Unretained(this), topic));
    return;
  }

  if (!IsRegistered()) {
    LOG(ERROR) << register_state_.InvalidStateError();
    return;
  }

  register_state_.ToUnregistering(FROM_HERE);

  OnUnubscribeTopicAsync(nullptr, nullptr, StatusOnceCallback(), Status::OK());
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnSubscribeTopicAsync(
    const SubscribeTopicRequest* request,
    const SubscribeTopicResponse* response, int channel_types,
    const communication::Settings& settings,
    OnMessageCallback on_message_callback, StatusCallback on_error_callback,
    StatusOnceCallback callback, Status s) {
  if (!IsRegistering()) {
    internal::LogOrCallback(std::move(callback),
                            register_state_.InvalidStateError());
    return;
  }

  if (!s.ok()) {
    register_state_.ToUnregistered(FROM_HERE);
    internal::LogOrCallback(std::move(callback), std::move(s));
    return;
  }

  channel_types_ = channel_types;
  on_message_callback_ = on_message_callback;
  on_error_callback_ = on_error_callback;
  settings_ = settings;

  register_state_.ToRegistered(FROM_HERE);
  internal::LogOrCallback(std::move(callback), std::move(s));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnUnubscribeTopicAsync(
    const UnsubscribeTopicRequest* request,
    const UnsubscribeTopicResponse* response, StatusOnceCallback callback,
    Status s) {
  if (!IsUnregistering()) {
    internal::LogOrCallback(std::move(callback),
                            register_state_.InvalidStateError());
    return;
  }

  if (!s.ok()) {
    register_state_.ToRegistered(FROM_HERE);
    internal::LogOrCallback(std::move(callback), std::move(s));
    return;
  }

  register_state_.ToUnregistered(FROM_HERE);
  StopMessageLoop();
  internal::LogOrCallback(std::move(callback), std::move(s));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnFindPublisher(const TopicInfo& topic_info) {
  MainThread& main_thread = MainThread::GetInstance();
  DCHECK(main_thread.IsBoundToCurrentThread());
  if (IsRegistering() || IsUnregistering() || IsStopping()) {
    main_thread.PostTask(FROM_HERE,
                         base::BindOnce(&Subscriber<MessageTy>::OnFindPublisher,
                                        base::Unretained(this), topic_info));
    return;
  }

  if (IsUnregistered()) return;

  DCHECK(IsRegistered()) << register_state_.ToString();

  if (topic_info.status() == TopicInfo::UNREGISTERED) {
    if (IsStarted()) {
      StopMessageLoop();
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

  // If MessageTy is DynamicProtobufMessage, in other words, this class is
  // a instance of DynamicSubscriber, then subscriber resolves its type
  // using |type_name| inside |topic_info|.
  if (!MaybeResolveMessgaeType(topic_info)) return;

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
    if (matched_channel_def.type() != ChannelDef::CHANNEL_TYPE_NONE) break;
    channel_type_ <<= 1;
  }

  if (matched_channel_def.type() == ChannelDef::CHANNEL_TYPE_NONE) {
    channel_.reset();
    internal::LogOrCallback(
        on_error_callback_,
        errors::Unavailable("Failed to connect to publisher."));
    return;
  }

  channel_ = ChannelFactory::NewChannel(matched_channel_def.type(),
                                        settings_.channel_settings);

  channel_->Connect(matched_channel_def,
                    base::BindOnce(&Subscriber<MessageTy>::OnConnectToPublisher,
                                   base::Unretained(this)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnConnectToPublisher(Status s) {
  if (s.ok()) {
    if (settings_.is_dynamic_buffer) {
      channel_->SetDynamicReceiveBuffer(true);
    } else {
      channel_->SetReceiveBufferSize(settings_.buffer_size);
    }
    message_receiver_.set_channel(channel_.get());

#if defined(HAS_ROS)
    if (IsUsingRosProtocol(topic_info_.topic())) {
      topic_request_.Request(
          this, base::BindOnce(&Subscriber<MessageTy>::OnRosTopicHandshake,
                               base::Unretained(this)));
    } else {
#endif  // defined(HAS_ROS)
      StartMessageLoop();
#if defined(HAS_ROS)
    }
#endif  // defined(HAS_ROS)
  } else {
    LOG(ERROR) << "Failed to connect to publisher: " << s;
    channel_type_ <<= 1;
    ConnectToPublisher();
  }
}

#if defined(HAS_ROS)
template <typename MessageTy>
void Subscriber<MessageTy>::OnRosTopicHandshake(Status s) {
  if (s.ok()) {
    channel_->SetDynamicReceiveBuffer(false);
    StartMessageLoop();
  } else {
    LOG(ERROR) << "Failed to connect to publisher: " << s;
    channel_type_ <<= 1;
    ConnectToPublisher();
  }
}
#endif  // defined(HAS_ROS)

template <typename MessageTy>
void Subscriber<MessageTy>::StartMessageLoop() {
  MainThread& main_thread = MainThread::GetInstance();
  if (!main_thread.IsBoundToCurrentThread()) {
    main_thread.PostTask(
        FROM_HERE, base::BindOnce(&Subscriber<MessageTy>::StartMessageLoop,
                                  base::Unretained(this)));
    return;
  }

  if (IsUnregistering() || IsUnregistered()) return;

  if (IsStopping()) {
    main_thread.PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&Subscriber<MessageTy>::StartMessageLoop,
                       base::Unretained(this)),
        settings_.period);
    return;
  }

  if (IsStarted()) return;

  subscriber_state_.ToStarted(FROM_HERE);
  message_queue_.reserve(settings_.queue_size);
  ReceiveMessageLoop();
  NotifyMessageLoop();
}

template <typename MessageTy>
void Subscriber<MessageTy>::StopMessageLoop(StatusOnceCallback callback) {
  MainThread& main_thread = MainThread::GetInstance();
  if (!main_thread.IsBoundToCurrentThread()) {
    main_thread.PostTask(
        FROM_HERE, base::BindOnce(&Subscriber<MessageTy>::StopMessageLoop,
                                  base::Unretained(this), std::move(callback)));
    return;
  }

  if (IsStopped() || IsStopping()) {
    LOG(ERROR) << "Tried stopping again";
    if (!callback.is_null())
      std::move(callback).Run(errors::Aborted("Already stopping or stopped."));
    return;
  }

  on_stopped_callback_ = std::move(callback);
  subscriber_state_.ToStopping(FROM_HERE);
}

template <typename MessageTy>
void Subscriber<MessageTy>::ReceiveMessageLoop() {
  if (IsStopping() || IsStopped()) return;

  message_receiver_.ReceiveMessage(base::BindOnce(
      &Subscriber<MessageTy>::OnReceiveMessage, base::Unretained(this)));
}

template <typename MessageTy>
void Subscriber<MessageTy>::OnReceiveMessage(Status s) {
  if (IsStopping() || IsStopped()) return;

  if (s.ok()) {
    receive_message_failed_cnt_ = 0;
    message_queue_.push(std::move(message_receiver_).message());
  } else {
    Status new_status(s.error_code(),
                      base::StringPrintf("Failed to receive a message: %s",
                                         s.error_message().c_str()));
    internal::LogOrCallback(on_error_callback_, new_status);
    if (channel_->IsTCPChannel() && !channel_->ToTCPChannel()->IsConnected()) {
      StopMessageLoop();
      return;
    }
    receive_message_failed_cnt_++;
    if (receive_message_failed_cnt_ >= kMaximumReceiveMessageFailedAllowed) {
      StopMessageLoop();
      return;
    }
  }

  if (channel_->IsShmChannel()) {
    MainThread& main_thread = MainThread::GetInstance();
    main_thread.PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&Subscriber<MessageTy>::ReceiveMessageLoop,
                       base::Unretained(this)),
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

  MainThread& main_thread = MainThread::GetInstance();
  if (IsStopping() && message_queue_.empty()) {
    main_thread.PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&Subscriber<MessageTy>::Stop, base::Unretained(this)),
        settings_.period + base::TimeDelta::FromMilliseconds(
                               100));  // Add some offset for safe close.
  } else {
    main_thread.PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&Subscriber<MessageTy>::NotifyMessageLoop,
                       base::Unretained(this)),
        settings_.period);
  }
}

// Should carefully release the resources.
// |channel_| should be released on main thread,
// if you release |message_queue_|, |on_message_callback_| and
// |on_error_callback_| too early, then it might be crashed on
// |NotifyMessageLoop|, which loops every |settings_.period|.
template <typename MessageTy>
void Subscriber<MessageTy>::Stop() {
#if DCHECK_IS_ON()
  MainThread& main_thread = MainThread::GetInstance();
  DCHECK(main_thread.IsBoundToCurrentThread());
#endif

  if (IsUnregistered()) {
    topic_info_.Clear();
    if (topic_info_to_update_.has_value()) {
      topic_info_to_update_.value().Clear();
    }
    message_receiver_.Reset();
    channel_.reset();
    message_queue_.clear();
#if defined(HAS_ROS)
    topic_request_.Reset();
#endif

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