#include "felicia/core/master/client/topic_source_watcher.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/net/base/ip_endpoint.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/master/rpc/grpc_util.h"

namespace felicia {

void TopicSourceWatcher::Listen(const std::string& topic,
                                OnNewTopicSourceCallback callback,
                                const ChannelDef& channel_def) {
  callback_map_[topic] = callback;

  if (!channel_) {
    channel_ = ChannelFactory::NewChannel<TopicSource>(channel_def);

    if (channel_->IsTCPChannel()) {
      task_runner_interface_->PostTask(
          FROM_HERE, ::base::BindOnce(&TopicSourceWatcher::DoListen,
                                      ::base::Unretained(this)));
    } else {
      NOTIMPLEMENTED();
    }
  }
}

void TopicSourceWatcher::DoListen() {
  LOG(INFO) << "TopicSourceWatcher::DoListen()" << node_info_.DebugString();
  TCPChannel<TopicSource>* tcp_channel = channel_->ToTCPChannel();
  tcp_channel->Listen(
      node_info_,
      ::base::BindOnce(&TopicSourceWatcher::OnListen, ::base::Unretained(this)),
      ::base::BindRepeating(&TopicSourceWatcher::OnAccept,
                            ::base::Unretained(this)));
}

void TopicSourceWatcher::OnListen(
    const StatusOr<::net::IPEndPoint>& status_or) {
  LOG(INFO) << "TopicSourceWatcher::OnListen() "
            << status_or.ValueOrDie().ToString();
}

void TopicSourceWatcher::OnAccept(const Status& s) {
  if (s.ok()) {
    WatchNewTopicSource();
  } else {
    LOG(ERROR) << "Failed to accept: " << s.error_message();
  }
}

void TopicSourceWatcher::WatchNewTopicSource() {
  LOG(INFO) << "TopicSourceWatcher::WatchNewTopicSource()";
  DCHECK(channel_);
  channel_->ReceiveMessage(
      &topic_source_, ::base::BindOnce(&TopicSourceWatcher::OnNewTopicSource,
                                       ::base::Unretained(this)));
}

void TopicSourceWatcher::OnNewTopicSource(const Status& s) {
  if (s.ok()) {
    LOG(INFO) << "TopicSourceWatcher::OnNewTopicSource(): "
              << topic_source_.DebugString();
    auto it = callback_map_.find(topic_source_.topic());
    if (it != callback_map_.end()) {
      it->second.Run(topic_source_);
    }
  }
}

}  // namespace felicia