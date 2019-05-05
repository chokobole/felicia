#include "felicia/core/master/tool/command_dispatcher.h"

#include <iostream>
#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/lib/felicia_env.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/core/node/dynamic_publishing_node.h"
#include "felicia/core/node/dynamic_subscribing_node.h"
#include "felicia/core/util/command_line_interface/table_writer.h"

namespace felicia {

namespace {

constexpr size_t kIdLength = 13;
constexpr size_t kNameLength = 20;
constexpr size_t kTopicNameLength = 20;
constexpr size_t kMessageTypeLength = 30;
constexpr size_t kChannelProtocolLength = 12;
constexpr size_t kChannelSourceLength = 22;

class ScopedMasterProxyStopper {
 public:
  ScopedMasterProxyStopper() {}
  ~ScopedMasterProxyStopper() {
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.Stop();
  }
};

}  // namespace

CommandDispatcher::CommandDispatcher() {}

void CommandDispatcher::Dispatch(const CliFlag& delegate) const {
  auto command = delegate.command();
  switch (command) {
    case CliFlag::Command::COMMAND_SELF:
      break;
    case CliFlag::Command::COMMAND_CLIENT:
      Dispatch(delegate.client_delegate());
      break;
    case CliFlag::Command::COMMAND_NODE:
      Dispatch(delegate.node_delegate());
      break;
    case CliFlag::Command::COMMAND_TOPIC:
      Dispatch(delegate.topic_delegate());
      break;
  }
}

void CommandDispatcher::Dispatch(const ClientFlag& delegate) const {
  auto command = delegate.command();
  switch (command) {
    case ClientFlag::Command::COMMAND_SELF:
      NOTREACHED();
      break;
    case ClientFlag::Command::COMMAND_LIST:
      Dispatch(delegate.list_delegate());
      break;
  }
}

void CommandDispatcher::Dispatch(const ClientListFlag& delegate) const {
  ClientFilter client_filter;
  if (delegate.all_flag()->is_set()) {
    client_filter.set_all(delegate.all_flag()->value());
  } else {
    DCHECK(!delegate.id_flag()->is_set());
    client_filter.set_id(delegate.id_flag()->value());
  }
  ListClientsRequest* request = new ListClientsRequest();
  *request->mutable_client_filter() = client_filter;
  ListClientsResponse* response = new ListClientsResponse();

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.ListClientsAsync(
      request, response,
      ::base::BindOnce(&CommandDispatcher::OnListClientsAsync,
                       ::base::Unretained(this), ::base::Owned(request),
                       ::base::Owned(response)));
}

void CommandDispatcher::OnListClientsAsync(ListClientsRequest* request,
                                           ListClientsResponse* response,
                                           const Status& s) const {
  ScopedMasterProxyStopper stopper;

  if (!s.ok()) {
    std::cerr << s.error_message() << std::endl;
    return;
  }
  auto client_infos = response->client_infos();
  TableWriterBuilder builder;
  auto writer = builder.AddColumn(TableWriter::Column{"ID", kIdLength})
                    .AddColumn(TableWriter::Column{"HEART_BEAT_SIGNALLER",
                                                   kChannelSourceLength})
                    .AddColumn(TableWriter::Column{"TOPIC_INFO_WATCHER",
                                                   kChannelSourceLength})
                    .Build();
  size_t row = 0;
  for (auto& client_info : client_infos) {
    writer.SetElement(row, 0, ::base::NumberToString(client_info.id()));
    const ChannelSource& hbs_channel_source =
        client_info.heart_beat_signaller_source();
    writer.SetElement(row, 1, ChannelSourceToString(hbs_channel_source));
    const ChannelSource& tiw_channel_source =
        client_info.topic_info_watcher_source();
    writer.SetElement(row, 2, ChannelSourceToString(tiw_channel_source));
    row++;
  }

  std::cout << writer.ToString() << std::endl;
}

void CommandDispatcher::Dispatch(const NodeFlag& delegate) const {
  auto command = delegate.command();
  switch (command) {
    case NodeFlag::Command::COMMAND_SELF:
      NOTREACHED();
      break;
    case NodeFlag::Command::COMMAND_LIST:
      Dispatch(delegate.list_delegate());
      break;
  }
}

void CommandDispatcher::Dispatch(const NodeListFlag& delegate) const {
  NodeFilter node_filter;
  if (delegate.all_flag()->is_set()) {
    node_filter.set_all(delegate.all_flag()->value());
  } else if (delegate.publishing_topic_flag()->is_set()) {
    node_filter.set_publishing_topic(delegate.publishing_topic_flag()->value());
  } else if (delegate.subscribing_topic_flag()->is_set()) {
    node_filter.set_subscribing_topic(
        delegate.subscribing_topic_flag()->value());
  } else {
    DCHECK(delegate.name_flag()->is_set());
    node_filter.set_name(delegate.name_flag()->value());
  }
  ListNodesRequest* request = new ListNodesRequest();
  *request->mutable_node_filter() = node_filter;
  ListNodesResponse* response = new ListNodesResponse();

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.ListNodesAsync(
      request, response,
      ::base::BindOnce(&CommandDispatcher::OnListNodesAsync,
                       ::base::Unretained(this), ::base::Owned(request),
                       ::base::Owned(response)));
}

void CommandDispatcher::OnListNodesAsync(ListNodesRequest* request,
                                         ListNodesResponse* response,
                                         const Status& s) const {
  ScopedMasterProxyStopper stopper;

  if (!s.ok()) {
    std::cerr << s.error_message() << std::endl;
    return;
  }

  TableWriterBuilder builder;
  if (response->has_pub_sub_topics()) {
    auto writer = builder.AddColumn(TableWriter::Column{"TOPIC", 20})
                      .AddColumn(TableWriter::Column{"TYPE", 10})
                      .Build();

    auto& pub_sub_topics = response->pub_sub_topics();
    size_t row = 0;
    for (auto& publishing_topic : pub_sub_topics.publishing_topics()) {
      writer.SetElement(row, 0, publishing_topic);
      writer.SetElement(row, 1, "Publishing");
      row++;
    }

    for (auto& subscribing_topic : pub_sub_topics.subscribing_topics()) {
      writer.SetElement(row, 0, subscribing_topic);
      writer.SetElement(row, 1, "Subscribing");
      row++;
    }

    std::cout << writer.ToString() << std::endl << std::endl;
  } else {
    auto& node_infos = response->node_infos();
    auto writer = builder.AddColumn(TableWriter::Column{"NAME", kNameLength})
                      .AddColumn(TableWriter::Column{"CLIENT ID", kIdLength})
                      .Build();
    size_t row = 0;
    for (auto& node_info : node_infos) {
      writer.SetElement(row, 0, node_info.name());
      writer.SetElement(row, 1, ::base::NumberToString(node_info.client_id()));
      row++;
    }

    std::cout << writer.ToString() << std::endl;
  }
}

void CommandDispatcher::Dispatch(const TopicFlag& delegate) const {
  auto command = delegate.command();
  switch (command) {
    case TopicFlag::Command::COMMAND_SELF:
      NOTREACHED();
      break;
    case TopicFlag::Command::COMMAND_LIST:
      Dispatch(delegate.list_delegate());
      break;
    case TopicFlag::Command::COMMAND_PUBLISH:
      Dispatch(delegate.publish_delegate());
      break;
    case TopicFlag::Command::COMMAND_SUBSCRIBE:
      Dispatch(delegate.subscribe_delegate());
      break;
  }
}

void CommandDispatcher::Dispatch(const TopicListFlag& delegate) const {
  TopicFilter topic_filter;
  if (delegate.all_flag()->is_set()) {
    topic_filter.set_all(delegate.all_flag()->value());
  } else {
    DCHECK(delegate.topic_flag()->is_set());
    topic_filter.set_topic(delegate.topic_flag()->value());
  }
  ListTopicsRequest* request = new ListTopicsRequest();
  *request->mutable_topic_filter() = topic_filter;
  ListTopicsResponse* response = new ListTopicsResponse();

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.ListTopicsAsync(
      request, response,
      ::base::BindOnce(&CommandDispatcher::OnListTopicsAsync,
                       ::base::Unretained(this), ::base::Owned(request),
                       ::base::Owned(response)));
}

void CommandDispatcher::Dispatch(const TopicPublishFlag& delegate) const {
  MasterProxy& master_proxy = MasterProxy::GetInstance();

  NodeInfo node_info;

  protobuf_loader_ = ProtobufLoader::Load(
      ::base::FilePath(FILE_PATH_LITERAL("") FELICIA_ROOT));

  ChannelDef channel_def;
  channel_def.set_type(ChannelDef::TCP);

  master_proxy.RequestRegisterNode<DynamicPublishingNode>(
      node_info, protobuf_loader_.get(), delegate.topic_flag()->value(),
      delegate.type_flag()->value(), channel_def,
      ::base::BindOnce(
          &CommandDispatcher::PublishMessageFromJSON, ::base::Unretained(this),
          delegate.message_flag()->value(), delegate.interval_flag()->value()));
}

void CommandDispatcher::Dispatch(const TopicSubscribeFlag& delegate) const {
  MasterProxy& master_proxy = MasterProxy::GetInstance();

  NodeInfo node_info;
  if (delegate.all_flag()->value()) node_info.set_watcher(true);

  protobuf_loader_ = ProtobufLoader::Load(
      ::base::FilePath(FILE_PATH_LITERAL("") FELICIA_ROOT));

  communication::Settings settings;
  if (delegate.period_flag()->is_set())
    settings.period =
        ::base::TimeDelta::FromMilliseconds(delegate.period_flag()->value());
  if (delegate.queue_size_flag()->is_set())
    settings.queue_size = delegate.queue_size_flag()->value();

  master_proxy.RequestRegisterNode<DynamicSubscribingNode>(
      node_info, protobuf_loader_.get(),
      ::base::BindRepeating(&CommandDispatcher::OnNewMessage,
                            ::base::Unretained(this)),
      ::base::BindRepeating(&CommandDispatcher::OnSubscriptionError,
                            ::base::Unretained(this)),
      settings, delegate.topic_flag()->value());
}

void CommandDispatcher::OnListTopicsAsync(ListTopicsRequest* request,
                                          ListTopicsResponse* response,
                                          const Status& s) const {
  ScopedMasterProxyStopper stopper;

  if (!s.ok()) {
    std::cerr << s.error_message() << std::endl;
    return;
  }

  auto topic_infos = response->topic_infos();
  if (topic_infos.size() == 0) return;

  if (request->topic_filter().all()) {
    TableWriterBuilder builder;
    auto writer =
        builder.AddColumn(TableWriter::Column{"NAME", kTopicNameLength})
            .AddColumn(TableWriter::Column{"TYPE", kMessageTypeLength})
            .AddColumn(TableWriter::Column{"PROTOCOL", kChannelProtocolLength})
            .AddColumn(TableWriter::Column{"END POINT", kChannelSourceLength})
            .Build();
    size_t row = 0;
    for (auto& topic_info : topic_infos) {
      writer.SetElement(row, 0, topic_info.topic());
      writer.SetElement(row, 1, topic_info.type_name());
      const ChannelSource& channel_source = topic_info.topic_source();
      writer.SetElement(
          row, 2, ChannelDef_Type_Name(channel_source.channel_def().type()));
      writer.SetElement(row, 3, ChannelSourceToString(channel_source));
      row++;
    }

    std::cout << writer.ToString() << std::endl;
  } else {
    auto topic_info = topic_infos[0];
    const ChannelSource& channel_source = topic_info.topic_source();

    protobuf_loader_ = ProtobufLoader::Load(
        ::base::FilePath(FILE_PATH_LITERAL("") FELICIA_ROOT));

    const ::google::protobuf::Message* message =
        protobuf_loader_->NewMessage(topic_info.type_name());

    std::cout << ::base::StringPrintf(
                     "TYPE: %s\n"
                     "FORMAT: %s\n"
                     "PROTOCOL: %s\n"
                     "END_POINT: %s\n",
                     topic_info.type_name().c_str(),
                     message->GetDescriptor()->DebugString().c_str(),
                     ChannelDef_Type_Name(channel_source.channel_def().type())
                         .c_str(),
                     ChannelSourceToString(channel_source).c_str())
              << std::endl;
  }
}

void CommandDispatcher::PublishMessageFromJSON(
    const std::string& message, int64_t delay,
    DynamicPublisher* publisher) const {
  if (publisher) {
    publisher->Publish(message, ::base::BindOnce(&CommandDispatcher::OnPublish,
                                                 ::base::Unretained(this)));
    if (!publisher->IsUnregistered()) {
      if (delay > 0) {
        MasterProxy& master_proxy = MasterProxy::GetInstance();
        master_proxy.PostDelayedTask(
            FROM_HERE,
            ::base::BindOnce(&CommandDispatcher::PublishMessageFromJSON,
                             ::base::Unretained(this), message, delay,
                             ::base::Unretained(publisher)),
            ::base::TimeDelta::FromMilliseconds(delay));
      }
    }

  } else {
    LOG(ERROR) << "Failed to request publish or memory was corrupted while "
                  "publishing message.";
  }
}

void CommandDispatcher::OnPublish(const Status& s) const {
  LOG_IF(ERROR, !s.ok()) << s.error_message();
}

void CommandDispatcher::OnNewMessage(const std::string& topic,
                                     DynamicProtobufMessage&& message) const {
  std::cout << TextStyle::Green(
                   ::base::StringPrintf("[TOPIC] %s", topic.c_str()))
            << std::endl;
  std::cout << message.ToString();
}

void CommandDispatcher::OnSubscriptionError(const std::string& topic,
                                            const Status& s) const {
  std::cout << TextStyle::Red(::base::StringPrintf("[TOPIC] %s", topic.c_str()))
            << std::endl;
  LOG(ERROR) << s.error_message();
}

}  // namespace felicia