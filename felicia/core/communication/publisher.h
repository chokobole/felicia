#ifndef FELICIA_CC_COMMUNICATION_PUBLISHER_H_
#define FELICIA_CC_COMMUNICATION_PUBLISHER_H_

#include <memory>
#include <string>

#if defined(HAS_ROS)
#include <ros/message_traits.h>
#endif

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/compiler_specific.h"
#include "third_party/chromium/base/containers/stack_container.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/base/synchronization/lock.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/communication/register_state.h"
#include "felicia/core/communication/settings.h"
#include "felicia/core/lib/containers/pool.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/master_proxy.h"

namespace felicia {

template <typename MessageTy>
class Publisher {
 public:
  Publisher() = default;
  virtual ~Publisher() {
    DCHECK(IsUnregistered()) << register_state_.ToString();
  }

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

  void RequestPublish(const NodeInfo& node_info, const std::string& topic,
                      int channel_types,
                      const communication::Settings& settings,
                      StatusOnceCallback callback = StatusOnceCallback());

  void Publish(const MessageTy& message,
               SendMessageCallback callback = SendMessageCallback());
  void Publish(MessageTy&& message,
               SendMessageCallback callback = SendMessageCallback());

  void RequestUnpublish(const NodeInfo& node_info, const std::string& topic,
                        StatusOnceCallback callback = StatusOnceCallback());

 protected:
  void OnPublishTopicAsync(PublishTopicRequest* request,
                           PublishTopicResponse* response,
                           const communication::Settings& settings,
                           StatusOnceCallback callback, const Status& s);

  void OnUnpublishTopicAsync(UnpublishTopicRequest* request,
                             UnpublishTopicResponse* response,
                             StatusOnceCallback callback, const Status& s);

  StatusOr<ChannelDef> Setup(Channel<MessageTy>* channel_def,
                             const channel::Settings& settings);

  void SendMesasge(SendMessageCallback callback);
  void OnAccept(const Status& s);

  void Release();

  // Because type of DynamicProtobufMessage or SerializedMessage can't be
  // determined at compile time. We should workaround by doing runtime
  // asking its Publisher.
  virtual std::string GetMessageTypeName() const {
    return MessageIOImpl<MessageTy>::TypeName();
  }

  virtual TopicInfo::ImplType GetMessageImplType() const {
#if defined(HAS_ROS)
    if (ros::message_traits::IsMessage<MessageTy>::value) {
      return TopicInfo::ROS;
    }
#endif
    return TopicInfo::PROTOBUF;
  }

  // Because SerializedMessage holds serialized text already, and we want it
  // to move its content to |serialized| not by copying.
  virtual MessageIOError SerializeToString(MessageTy* message,
                                           std::string* serialized) {
    return MessageIO::SerializeToString(message, serialized);
  }

  base::Lock lock_;
  std::unique_ptr<Pool<MessageTy, uint8_t>> message_queue_ GUARDED_BY(lock_);
  base::TimeDelta period_;
  std::vector<std::unique_ptr<Channel<MessageTy>>> channels_;

  communication::RegisterState register_state_;

  DISALLOW_COPY_AND_ASSIGN(Publisher);
};

template <typename MessageTy>
void Publisher<MessageTy>::RequestPublish(
    const NodeInfo& node_info, const std::string& topic, int channel_types,
    const communication::Settings& settings, StatusOnceCallback callback) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE,
        base::BindOnce(&Publisher<MessageTy>::RequestPublish,
                       base::Unretained(this), node_info, topic, channel_types,
                       settings, std::move(callback)));
    return;
  }

  if (!IsUnregistered()) {
    internal::LogOrCallback(std::move(callback),
                            register_state_.InvalidStateError());
    return;
  }

  register_state_.ToRegistering(FROM_HERE);

  base::StackVector<ChannelDef, ChannelDef::Type_ARRAYSIZE> channel_defs;
  int channel_type = 1;
  while (channel_type <= channel_types) {
    if (channel_type & channel_types) {
      auto channel = ChannelFactory::NewChannel<MessageTy>(
          static_cast<ChannelDef::Type>(channel_type),
          settings.channel_settings);
      StatusOr<ChannelDef> status_or =
          Setup(channel.get(), settings.channel_settings);
      if (!status_or.ok()) {
        register_state_.ToUnregistered(FROM_HERE);
        Release();
        internal::LogOrCallback(std::move(callback), status_or.status());
        return;
      }
      channel_defs->push_back(status_or.ValueOrDie());
      channels_.push_back(std::move(channel));
    }
    channel_type <<= 1;
  }

  PublishTopicRequest* request = new PublishTopicRequest();
  *request->mutable_node_info() = node_info;
  TopicInfo* topic_info = request->mutable_topic_info();
  topic_info->set_topic(topic);
  topic_info->set_type_name(GetMessageTypeName());
  topic_info->set_impl_type(GetMessageImplType());
  ChannelSource* channel_source = topic_info->mutable_topic_source();
  for (auto& channel_def : channel_defs) {
    *channel_source->add_channel_defs() = channel_def;
  }
  PublishTopicResponse* response = new PublishTopicResponse();

  master_proxy.PublishTopicAsync(
      request, response,
      base::BindOnce(&Publisher<MessageTy>::OnPublishTopicAsync,
                     base::Unretained(this), base::Owned(request),
                     base::Owned(response), settings, std::move(callback)));
}

template <typename MessageTy>
void Publisher<MessageTy>::Publish(const MessageTy& message,
                                   SendMessageCallback callback) {
  {
    base::AutoLock l(lock_);
    if (message_queue_) message_queue_->push(message);
  }

  SendMesasge(callback);
}

template <typename MessageTy>
void Publisher<MessageTy>::Publish(MessageTy&& message,
                                   SendMessageCallback callback) {
  {
    base::AutoLock l(lock_);
    if (message_queue_) message_queue_->push(std::move(message));
  }

  SendMesasge(callback);
}

template <typename MessageTy>
void Publisher<MessageTy>::RequestUnpublish(const NodeInfo& node_info,
                                            const std::string& topic,
                                            StatusOnceCallback callback) {
  if (!IsRegistered()) {
    internal::LogOrCallback(std::move(callback),
                            register_state_.InvalidStateError());
    return;
  }

  register_state_.ToUnregistering(FROM_HERE);

  UnpublishTopicRequest* request = new UnpublishTopicRequest();
  *request->mutable_node_info() = node_info;
  request->set_topic(topic);
  UnpublishTopicResponse* response = new UnpublishTopicResponse();

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.UnpublishTopicAsync(
      request, response,
      base::BindOnce(&Publisher<MessageTy>::OnUnpublishTopicAsync,
                     base::Unretained(this), base::Owned(request),
                     base::Owned(response), std::move(callback)));
}

template <typename MessageTy>
StatusOr<ChannelDef> Publisher<MessageTy>::Setup(
    Channel<MessageTy>* channel, const channel::Settings& settings) {
  if (!channel) {
    return errors::Aborted("Failed to setup: channel is null.");
  }

  StatusOr<ChannelDef> status_or;
  if (channel->IsTCPChannel()) {
    TCPChannel<MessageTy>* tcp_channel = channel->ToTCPChannel();
    status_or = tcp_channel->Listen();
    tcp_channel->AcceptLoop(base::BindRepeating(
        [](const Status& s) { LOG_IF(ERROR, !s.ok()) << s; }));
  } else if (channel->IsUDPChannel()) {
    UDPChannel<MessageTy>* udp_channel = channel->ToUDPChannel();
    status_or = udp_channel->Bind();
  } else if (channel->IsWSChannel()) {
    WSChannel<MessageTy>* ws_channel = channel->ToWSChannel();
    status_or = ws_channel->Listen();
    ws_channel->AcceptLoop(base::BindRepeating(
        [](const Status& s) { LOG_IF(ERROR, !s.ok()) << s; }));
  }
#if defined(OS_POSIX)
  else if (channel->IsUDSChannel()) {
    UDSChannel<MessageTy>* uds_channel = channel->ToUDSChannel();
    status_or = uds_channel->BindAndListen();
    uds_channel->AcceptLoop(base::BindRepeating([](const Status& s) {
                              LOG_IF(ERROR, !s.ok()) << s;
                            }),
                            settings.uds_settings.auth_callback);
  }
#endif
  else if (channel->IsShmChannel()) {
    ShmChannel<MessageTy>* shm_channel = channel->ToShmChannel();
    status_or = shm_channel->MakeSharedMemory();
  }

  return status_or;
}

template <typename MessageTy>
void Publisher<MessageTy>::OnPublishTopicAsync(
    PublishTopicRequest* request, PublishTopicResponse* response,
    const communication::Settings& settings, StatusOnceCallback callback,
    const Status& s) {
  if (!IsRegistering()) {
    internal::LogOrCallback(std::move(callback),
                            register_state_.InvalidStateError());
    return;
  }

  if (!s.ok()) {
    register_state_.ToUnregistered(FROM_HERE);
    internal::LogOrCallback(std::move(callback), s);
    return;
  }

  period_ = settings.period;
  {
    base::AutoLock l(lock_);
    message_queue_ =
        std::make_unique<Pool<MessageTy, uint8_t>>(settings.queue_size);
  }

  SendBuffer send_buffer;
  if (!settings.is_dynamic_buffer) {
    send_buffer.SetCapacity(settings.buffer_size);
  }
  for (auto& channel : channels_) {
    channel->SetSendBuffer(send_buffer);
    if (settings.is_dynamic_buffer) {
      channel->EnableDynamicBuffer();
    }
  }

  register_state_.ToRegistered(FROM_HERE);
  internal::LogOrCallback(std::move(callback), s);
}

template <typename MessageTy>
void Publisher<MessageTy>::OnUnpublishTopicAsync(
    UnpublishTopicRequest* request, UnpublishTopicResponse* response,
    StatusOnceCallback callback, const Status& s) {
  if (!IsUnregistering()) {
    internal::LogOrCallback(std::move(callback),
                            register_state_.InvalidStateError());
    return;
  }

  if (!s.ok()) {
    register_state_.ToRegistered(FROM_HERE);
    internal::LogOrCallback(std::move(callback), s);
    return;
  }

  register_state_.ToUnregistered(FROM_HERE);
  Release();

  internal::LogOrCallback(std::move(callback), s);
}

template <typename MessageTy>
void Publisher<MessageTy>::SendMesasge(SendMessageCallback callback) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(FROM_HERE,
                          base::BindOnce(&Publisher<MessageTy>::SendMesasge,
                                         base::Unretained(this), callback));
    return;
  }

  bool can_send = false;
  for (auto& channel : channels_) {
    if (!channel->IsSendingMessage() && channel->HasReceivers()) {
      can_send = true;
      break;
    }
  }

  if (!can_send) return;

  std::string serialized;
  MessageIOError err;
  {
    base::AutoLock l(lock_);
    if (message_queue_ && !message_queue_->empty()) {
      err = SerializeToString(&message_queue_->front(), &serialized);
      message_queue_->pop();
    }
  }
  if (err == MessageIOError::OK) {
    bool reuse = false;
    for (auto& channel : channels_) {
      if (!channel->IsSendingMessage() && channel->HasReceivers()) {
        channel->SendMessage(serialized, reuse, callback);
        reuse |= true;
      }
    }
  }

  master_proxy.PostDelayedTask(
      FROM_HERE,
      base::BindOnce(&Publisher<MessageTy>::SendMesasge, base::Unretained(this),
                     callback),
      period_);
}

template <typename MessageTy>
void Publisher<MessageTy>::Release() {
  DCHECK(IsUnregistered());
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE,
        base::BindOnce(&Publisher<MessageTy>::Release, base::Unretained(this)));
    return;
  }

  channels_.clear();
  {
    base::AutoLock l(lock_);
    message_queue_.reset();
  }
}

}  // namespace felicia

#endif  // FELICIA_CC_COMMUNICATION_PUBLISHER_H_