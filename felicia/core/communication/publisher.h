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
#include "felicia/core/communication/state.h"
#include "felicia/core/lib/containers/pool.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/master_proxy.h"

namespace felicia {

template <typename MessageTy>
class Publisher {
 public:
  Publisher() { state_.ToUneregistered(); }

  ~Publisher() { DCHECK(IsUnregistered()); }

  ALWAYS_INLINE bool IsRegistering() const { return state_.IsRegistering(); }
  ALWAYS_INLINE bool IsRegistered() const { return state_.IsRegistered(); }
  ALWAYS_INLINE bool IsUnregistering() const {
    return state_.IsUnregistering();
  }
  ALWAYS_INLINE bool IsUnregistered() const { return state_.IsUnregistered(); }

  void RequestPublish(const NodeInfo& node_info, const std::string& topic,
                      const ChannelDef& channel_def, StatusCallback callback);

  void Publish(const MessageTy& message, StatusCallback callback);
  void Publish(MessageTy&& message, StatusCallback callback);

  void RequestUnpublish(const NodeInfo& node_info, const std::string& topic,
                        StatusCallback callback);

 private:
  void OnPublishTopicAsync(PublishTopicRequest* request,
                           PublishTopicResponse* response,
                           StatusCallback callback, const Status& s);

  void OnUnpublishTopicAsync(UnpublishTopicRequest* request,
                             UnpublishTopicResponse* response,
                             StatusCallback callback, const Status& s);

  StatusOr<ChannelSource> Setup(const ChannelDef& channel_def);

  void SendMesasge(StatusCallback callback);
  void DoAcceptLoop();
  void OnAccept(const Status& s);

  void Release();

  Pool<MessageTy, uint8_t> message_queue_;
  std::unique_ptr<Channel<MessageTy>> channel_;

  communication::State state_;

  DISALLOW_COPY_AND_ASSIGN(Publisher);
};

template <typename MessageTy>
void Publisher<MessageTy>::RequestPublish(const NodeInfo& node_info,
                                          const std::string& topic,
                                          const ChannelDef& channel_def,
                                          StatusCallback callback) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE, ::base::BindOnce(&Publisher<MessageTy>::RequestPublish,
                                    ::base::Unretained(this), node_info, topic,
                                    channel_def, std::move(callback)));
    return;
  }

  if (!IsUnregistered()) {
    std::move(callback).Run(state_.InvalidStateError());
    return;
  }

  state_.ToRegistering();

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
  *topic_info->mutable_topic_source() = status_or.ValueOrDie();
  PublishTopicResponse* response = new PublishTopicResponse();

  master_proxy.PublishTopicAsync(
      request, response,
      ::base::BindOnce(&Publisher<MessageTy>::OnPublishTopicAsync,
                       ::base::Unretained(this), ::base::Owned(request),
                       ::base::Owned(response), std::move(callback)));
}

template <typename MessageTy>
void Publisher<MessageTy>::Publish(const MessageTy& message,
                                   StatusCallback callback) {
  if (!IsRegistered()) {
    std::move(callback).Run(state_.InvalidStateError());
    return;
  }

  message_queue_.push(message);

  SendMesasge(std::move(callback));
}

template <typename MessageTy>
void Publisher<MessageTy>::Publish(MessageTy&& message,
                                   StatusCallback callback) {
  if (!IsRegistered()) {
    std::move(callback).Run(state_.InvalidStateError());
    return;
  }

  message_queue_.push(std::move(message));

  SendMesasge(std::move(callback));
}

template <typename MessageTy>
void Publisher<MessageTy>::RequestUnpublish(const NodeInfo& node_info,
                                            const std::string& topic,
                                            StatusCallback callback) {
  if (!IsRegistered()) {
    std::move(callback).Run(state_.InvalidStateError());
    return;
  }

  state_.ToUnregistering();

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
void Publisher<MessageTy>::OnPublishTopicAsync(PublishTopicRequest* request,
                                               PublishTopicResponse* response,
                                               StatusCallback callback,
                                               const Status& s) {
  DCHECK(IsRegistering());

  if (!s.ok()) {
    state_.ToUneregistered();
    std::move(callback).Run(s);
    return;
  }

  message_queue_.set_capacity(10);

  state_.ToRegistered();
  std::move(callback).Run(s);
}

template <typename MessageTy>
void Publisher<MessageTy>::OnUnpublishTopicAsync(
    UnpublishTopicRequest* request, UnpublishTopicResponse* response,
    StatusCallback callback, const Status& s) {
  DCHECK(IsUnregistering());

  if (!s.ok()) {
    state_.ToRegistered();
    std::move(callback).Run(s);
    return;
  }

  Release();
  state_.ToUneregistered();

  std::move(callback).Run(s);
}

template <typename MessageTy>
void Publisher<MessageTy>::SendMesasge(StatusCallback callback) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!channel_->IsSendingMessage() && master_proxy.IsBoundToCurrentThread()) {
    if (!message_queue_.empty()) {
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