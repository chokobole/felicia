#ifndef FELICIA_CC_COMMUNICATION_PUBLISHER_H_
#define FELICIA_CC_COMMUNICATION_PUBLISHER_H_

#include <memory>
#include <string>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/compiler_specific.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/stringprintf.h"

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
  virtual ~Publisher() { DCHECK(IsUnregistered()); }

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
                      const ChannelDef& channel_def,
                      const communication::Settings& settings,
                      StatusOnceCallback callback);

  void Publish(const MessageTy& message, StatusOnceCallback callback);
  void Publish(MessageTy&& message, StatusOnceCallback callback);

  void RequestUnpublish(const NodeInfo& node_info, const std::string& topic,
                        StatusOnceCallback callback);

 protected:
  void OnPublishTopicAsync(PublishTopicRequest* request,
                           PublishTopicResponse* response,
                           const communication::Settings& settings,
                           StatusOnceCallback callback, const Status& s);

  void OnUnpublishTopicAsync(UnpublishTopicRequest* request,
                             UnpublishTopicResponse* response,
                             StatusOnceCallback callback, const Status& s);

  StatusOr<ChannelSource> Setup(const ChannelDef& channel_def);

  void SendMesasge(StatusOnceCallback callback);
  void DoAcceptLoop();
  void OnAccept(const Status& s);

  void Release();

  // Because DynamicProtobufMessage's type can't be determined at compile time.
  // we should workaround by doing runtime asking DynamicPublisher.
  virtual std::string GetMessageTypeName() const {
    MessageTy message;
    return message.GetTypeName();
  }

  Pool<MessageTy, uint8_t> message_queue_;
  std::unique_ptr<Channel<MessageTy>> channel_;

  communication::RegisterState register_state_;

  DISALLOW_COPY_AND_ASSIGN(Publisher);
};

template <typename MessageTy>
void Publisher<MessageTy>::RequestPublish(
    const NodeInfo& node_info, const std::string& topic,
    const ChannelDef& channel_def, const communication::Settings& settings,
    StatusOnceCallback callback) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE,
        ::base::BindOnce(&Publisher<MessageTy>::RequestPublish,
                         ::base::Unretained(this), node_info, topic,
                         channel_def, settings, std::move(callback)));
    return;
  }

  if (!IsUnregistered()) {
    std::move(callback).Run(register_state_.InvalidStateError());
    return;
  }

  register_state_.ToRegistering();

  StatusOr<ChannelSource> status_or = Setup(channel_def);
  if (!status_or.ok()) {
    std::move(callback).Run(status_or.status());
    return;
  }
  DoAcceptLoop();

  PublishTopicRequest* request = new PublishTopicRequest();
  *request->mutable_node_info() = node_info;
  TopicInfo* topic_info = request->mutable_topic_info();
  topic_info->set_topic(topic);
  topic_info->set_type_name(GetMessageTypeName());
  *topic_info->mutable_topic_source() = status_or.ValueOrDie();
  PublishTopicResponse* response = new PublishTopicResponse();

  master_proxy.PublishTopicAsync(
      request, response,
      ::base::BindOnce(&Publisher<MessageTy>::OnPublishTopicAsync,
                       ::base::Unretained(this), ::base::Owned(request),
                       ::base::Owned(response), settings, std::move(callback)));
}

template <typename MessageTy>
void Publisher<MessageTy>::Publish(const MessageTy& message,
                                   StatusOnceCallback callback) {
  if (!IsRegistered()) {
    std::move(callback).Run(register_state_.InvalidStateError());
    return;
  }

  message_queue_.push(message);

  SendMesasge(std::move(callback));
}

template <typename MessageTy>
void Publisher<MessageTy>::Publish(MessageTy&& message,
                                   StatusOnceCallback callback) {
  if (!IsRegistered()) {
    std::move(callback).Run(register_state_.InvalidStateError());
    return;
  }

  message_queue_.push(std::move(message));

  SendMesasge(std::move(callback));
}

template <typename MessageTy>
void Publisher<MessageTy>::RequestUnpublish(const NodeInfo& node_info,
                                            const std::string& topic,
                                            StatusOnceCallback callback) {
  if (!IsRegistered()) {
    std::move(callback).Run(register_state_.InvalidStateError());
    return;
  }

  register_state_.ToUnregistering();

  UnpublishTopicRequest* request = new UnpublishTopicRequest();
  *request->mutable_node_info() = node_info;
  request->set_topic(topic);
  UnpublishTopicResponse* response = new UnpublishTopicResponse();

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.UnpublishTopicAsync(
      request, response,
      ::base::BindOnce(&Publisher<MessageTy>::OnUnpublishTopicAsync,
                       ::base::Unretained(this), ::base::Owned(request),
                       ::base::Owned(response), std::move(callback)));
}

template <typename MessageTy>
StatusOr<ChannelSource> Publisher<MessageTy>::Setup(
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

  return status_or;
}

template <typename MessageTy>
void Publisher<MessageTy>::OnPublishTopicAsync(
    PublishTopicRequest* request, PublishTopicResponse* response,
    const communication::Settings& settings, StatusOnceCallback callback,
    const Status& s) {
  if (!IsRegistering()) {
    std::move(callback).Run(register_state_.InvalidStateError());
    return;
  }

  if (!s.ok()) {
    register_state_.ToUnregistered();
    std::move(callback).Run(s);
    return;
  }

  message_queue_.set_capacity(settings.queue_size);
  if (settings.is_dynamic_buffer) {
    channel_->EnableDynamicBuffer();
  } else {
    channel_->SetSendBufferSize(settings.buffer_size);
  }

  register_state_.ToRegistered();
  std::move(callback).Run(s);
}

template <typename MessageTy>
void Publisher<MessageTy>::OnUnpublishTopicAsync(
    UnpublishTopicRequest* request, UnpublishTopicResponse* response,
    StatusOnceCallback callback, const Status& s) {
  if (!IsUnregistering()) {
    std::move(callback).Run(register_state_.InvalidStateError());
    return;
  }

  if (!s.ok()) {
    register_state_.ToRegistered();
    std::move(callback).Run(s);
    return;
  }

  Release();
  register_state_.ToUnregistered();

  std::move(callback).Run(s);
}

template <typename MessageTy>
void Publisher<MessageTy>::SendMesasge(StatusOnceCallback callback) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!channel_->IsSendingMessage() && master_proxy.IsBoundToCurrentThread()) {
    if (!message_queue_.empty()) {
      if (!channel_->HasReceivers()) return;

      MessageTy message = std::move(message_queue_.front());
      message_queue_.pop();
      channel_->SendMessage(message, std::move(callback));
    }
  } else {
    master_proxy.PostTask(
        FROM_HERE,
        ::base::BindOnce(&Publisher<MessageTy>::SendMesasge,
                         ::base::Unretained(this), std::move(callback)));
  }
}

template <typename MessageTy>
void Publisher<MessageTy>::DoAcceptLoop() {
#if DCHECK_IS_ON()
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  DCHECK(master_proxy.IsBoundToCurrentThread());
#endif
  if (!channel_->IsTCPChannel()) return;

  channel_->ToTCPChannel()->DoAcceptLoop(::base::BindRepeating(
      &Publisher<MessageTy>::OnAccept, ::base::Unretained(this)));
}

template <typename MessageTy>
void Publisher<MessageTy>::OnAccept(const Status& s) {
  LOG_IF(ERROR, !s.ok()) << s.error_message();
}

template <typename MessageTy>
void Publisher<MessageTy>::Release() {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(FROM_HERE,
                          ::base::BindOnce(&Publisher<MessageTy>::Release,
                                           ::base::Unretained(this)));
    return;
  }

  channel_.reset();
  message_queue_.clear();
}

}  // namespace felicia

#endif  // FELICIA_CC_COMMUNICATION_PUBLISHER_H_