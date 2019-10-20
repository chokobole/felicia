#include "felicia/examples/learn/topic/protobuf/cc/protobuf_publishing_node.h"

#include "felicia/core/master/master_proxy.h"

namespace felicia {

ProtobufPublishingNode::ProtobufPublishingNode(
    const TopicCreateFlag& topic_create_flag,
    SSLServerContext* ssl_server_context)
    : topic_create_flag_(topic_create_flag),
      topic_(topic_create_flag_.topic_flag()->value()),
      ssl_server_context_(ssl_server_context) {}

void ProtobufPublishingNode::OnInit() {
  std::cout << "ProtobufPublishingNode::OnInit()" << std::endl;
}

void ProtobufPublishingNode::OnDidCreate(const NodeInfo& node_info) {
  std::cout << "ProtobufPublishingNode::OnDidCreate()" << std::endl;
  node_info_ = node_info;
  RequestPublish();

  // MasterProxy& master_proxy = MasterProxy::GetInstance();
  // master_proxy.PostDelayedTask(
  //     FROM_HERE,
  //     base::BindOnce(&ProtobufPublishingNode::RequestUnpublish,
  //                      base::Unretained(this)),
  //     base::TimeDelta::FromSeconds(10));
}

void ProtobufPublishingNode::OnError(const Status& s) {
  std::cout << "ProtobufPublishingNode::OnError()" << std::endl;
  LOG(ERROR) << s;
}

void ProtobufPublishingNode::OnPublish(ChannelDef::Type type, const Status& s) {
  std::cout << "ProtobufPublishingNode::OnPublish() from "
            << ChannelDef::Type_Name(type) << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}

void ProtobufPublishingNode::OnRequestPublish(const Status& s) {
  std::cout << "ProtobufPublishingNode::OnRequestPublish()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
  RepeatingPublish();
}

void ProtobufPublishingNode::OnRequestUnpublish(const Status& s) {
  std::cout << "ProtobufPublishingNode::OnRequestUnpublish()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}

#if defined(OS_POSIX)
bool ProtobufPublishingNode::OnAuth(
    const UnixDomainServerSocket::Credentials& credentials) {
  LOG(INFO)
#if defined(OS_LINUX)
      << "pid: " << credentials.process_id << " "
#endif
      << "uid: " << credentials.user_id << " gid: " << credentials.group_id;
  return true;
}
#endif

void ProtobufPublishingNode::RequestPublish() {
  ChannelDef::Type channel_type;
  ChannelDef::Type_Parse(topic_create_flag_.channel_type_flag()->value(),
                         &channel_type);

  communication::Settings settings;
  Bytes size = Bytes::FromBytes(512);
  settings.buffer_size = size;
  if (channel_type == ChannelDef::CHANNEL_TYPE_TCP) {
    if (ssl_server_context_) {
      settings.channel_settings.tcp_settings.use_ssl = true;
      settings.channel_settings.tcp_settings.ssl_server_context =
          ssl_server_context_;
    }
  }
#if defined(OS_POSIX)
  else if (channel_type == ChannelDef::CHANNEL_TYPE_UDS) {
    settings.channel_settings.uds_settings.auth_callback = base::BindRepeating(
        &ProtobufPublishingNode::OnAuth, base::Unretained(this));
  }
#endif
  else if (channel_type == ChannelDef::CHANNEL_TYPE_SHM) {
    settings.channel_settings.shm_settings.shm_size = size;
  }

  publisher_.RequestPublish(
      node_info_, topic_, channel_type, settings,
      base::BindOnce(&ProtobufPublishingNode::OnRequestPublish,
                     base::Unretained(this)));
}

void ProtobufPublishingNode::RequestUnpublish() {
  publisher_.RequestUnpublish(
      node_info_, topic_,
      base::BindOnce(&ProtobufPublishingNode::OnRequestUnpublish,
                     base::Unretained(this)));
}

void ProtobufPublishingNode::RepeatingPublish() {
  publisher_.Publish(GenerateMessage(),
                     base::BindRepeating(&ProtobufPublishingNode::OnPublish,
                                         base::Unretained(this)));

  if (!publisher_.IsUnregistered()) {
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&ProtobufPublishingNode::RepeatingPublish,
                       base::Unretained(this)),
        base::TimeDelta::FromSeconds(1));
  }
}

SimpleMessage ProtobufPublishingNode::GenerateMessage() {
  static int id = 0;
  base::TimeDelta timestamp = timestamper_.timestamp();
  SimpleMessage message;
  message.set_id(id++);
  message.set_timestamp(timestamp.InMicroseconds());
  message.set_content("hello world");
  return message;
}

}  // namespace felicia