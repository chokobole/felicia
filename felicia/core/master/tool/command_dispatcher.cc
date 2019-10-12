#include "felicia/core/master/tool/command_dispatcher.h"

#include <iostream>
#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/core/util/command_line_interface/table_writer.h"

namespace felicia {

namespace {

constexpr size_t kIdLength = 13;
constexpr size_t kNameLength = 20;
constexpr size_t kTopicNameLength = 20;
constexpr size_t kServiceNameLength = 20;
constexpr size_t kMessageTypeLength = 30;
constexpr size_t kChannelProtocolLength = 20;
constexpr size_t kChannelSourceLength = 24;

class ScopedMasterProxyStopper {
 public:
  ScopedMasterProxyStopper() {}
  ~ScopedMasterProxyStopper() {
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.Stop();
  }
};

}  // namespace

CommandDispatcher::CommandDispatcher() = default;

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
    case CliFlag::Command::COMMAND_SERVICE:
      Dispatch(delegate.service_delegate());
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
      base::BindOnce(&CommandDispatcher::OnListClientsAsync,
                     base::Unretained(this), base::Owned(request),
                     base::Owned(response)));
}

void CommandDispatcher::OnListClientsAsync(const ListClientsRequest* request,
                                           ListClientsResponse* response,
                                           const Status& s) const {
  ScopedMasterProxyStopper stopper;

  if (!s.ok()) {
    std::cerr << s << std::endl;
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
    writer.SetElement(row, 0, base::NumberToString(client_info.id()));
    const ChannelSource& hbs_channel_source =
        client_info.heart_beat_signaller_source();
    writer.SetElement(row, 1,
                      EndPointToString(hbs_channel_source.channel_defs(0)));
    const ChannelSource& tiw_channel_source =
        client_info.master_notification_watcher_source();
    writer.SetElement(row, 2,
                      EndPointToString(tiw_channel_source.channel_defs(0)));
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
      base::BindOnce(&CommandDispatcher::OnListNodesAsync,
                     base::Unretained(this), base::Owned(request),
                     base::Owned(response)));
}

void CommandDispatcher::OnListNodesAsync(const ListNodesRequest* request,
                                         ListNodesResponse* response,
                                         const Status& s) const {
  ScopedMasterProxyStopper stopper;

  if (!s.ok()) {
    std::cerr << s << std::endl;
    return;
  }

  TableWriterBuilder builder;
  if (response->has_pub_sub_topics()) {
    auto writer =
        builder.AddColumn(TableWriter::Column{"TOPIC", kTopicNameLength})
            .AddColumn(TableWriter::Column{"TYPE", 15})
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
      writer.SetElement(row, 1, base::NumberToString(node_info.client_id()));
      row++;
    }

    std::cout << writer.ToString() << std::endl;
  }
}

void CommandDispatcher::Dispatch(const ServiceFlag& delegate) const {
  auto command = delegate.command();
  switch (command) {
    case ServiceFlag::Command::COMMAND_SELF:
      NOTREACHED();
      break;
    case ServiceFlag::Command::COMMAND_LIST:
      Dispatch(delegate.list_delegate());
      break;
  }
}

void CommandDispatcher::Dispatch(const ServiceListFlag& delegate) const {
  ServiceFilter service_filter;
  if (delegate.all_flag()->is_set()) {
    service_filter.set_all(delegate.all_flag()->value());
  } else {
    DCHECK(delegate.service_flag()->is_set());
    service_filter.set_service(delegate.service_flag()->value());
  }
  ListServicesRequest* request = new ListServicesRequest();
  *request->mutable_service_filter() = service_filter;
  ListServicesResponse* response = new ListServicesResponse();

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.ListServicesAsync(
      request, response,
      base::BindOnce(&CommandDispatcher::OnListServicesAsync,
                     base::Unretained(this), base::Owned(request),
                     base::Owned(response)));
}

void CommandDispatcher::OnListServicesAsync(const ListServicesRequest* request,
                                            ListServicesResponse* response,
                                            const Status& s) const {
  ScopedMasterProxyStopper stopper;

  if (!s.ok()) {
    std::cerr << s << std::endl;
    return;
  }

  auto service_infos = response->service_infos();
  if (service_infos.size() == 0) return;

  TableWriterBuilder builder;
  auto writer =
      builder.AddColumn(TableWriter::Column{"NAME", kServiceNameLength})
          .AddColumn(TableWriter::Column{"TYPE", kMessageTypeLength})
          .AddColumn(TableWriter::Column{"END POINT", kChannelSourceLength})
          .Build();
  size_t row = 0;
  for (auto& service_info : service_infos) {
    const ChannelSource& channel_source = service_info.service_source();
    for (auto& channel_def : channel_source.channel_defs()) {
      writer.SetElement(row, 0, service_info.service());
      writer.SetElement(row, 1, service_info.type_name());
      writer.SetElement(row, 2, EndPointToString(channel_def));
      row++;
    }
  }

  std::cout << writer.ToString() << std::endl;
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
      base::BindOnce(&CommandDispatcher::OnListTopicsAsync,
                     base::Unretained(this), base::Owned(request),
                     base::Owned(response)));
}

void CommandDispatcher::Dispatch(const TopicPublishFlag& delegate) const {
  topic_publish_command_dispatcher_.Dispatch(delegate);
}

void CommandDispatcher::Dispatch(const TopicSubscribeFlag& delegate) const {
  topic_subscribe_command_dispatcher_.Dispatch(delegate);
}

void CommandDispatcher::OnListTopicsAsync(const ListTopicsRequest* request,
                                          ListTopicsResponse* response,
                                          const Status& s) const {
  ScopedMasterProxyStopper stopper;

  if (!s.ok()) {
    std::cerr << s << std::endl;
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
      const ChannelSource& channel_source = topic_info.topic_source();
      for (auto& channel_def : channel_source.channel_defs()) {
        writer.SetElement(row, 0, topic_info.topic());
        writer.SetElement(row, 1, topic_info.type_name());
        writer.SetElement(row, 2, ChannelDef::Type_Name(channel_def.type()));
        writer.SetElement(row, 3, EndPointToString(channel_def));
        row++;
      }
    }

    std::cout << writer.ToString() << std::endl;
  } else {
    auto topic_info = topic_infos[0];
    const ChannelSource& channel_source = topic_info.topic_source();

    MasterProxy& master_proxy = MasterProxy::GetInstance();

    std::string definition = "Unknown";
    if (topic_info.impl_type() == TopicInfo::PROTOBUF) {
      const google::protobuf::Message* message =
          master_proxy.protobuf_loader()->NewMessage(topic_info.type_name());
      if (message) {
        definition = message->GetDescriptor()->DebugString();
      }
    }

    std::cout << base::StringPrintf(
                     "TYPE: %s\n"
                     "IMPL_TYPE: %s\n"
                     "DEFINITION: %s\n"
                     "CHANNEL_SOURCE: %s\n",
                     topic_info.type_name().c_str(),
                     TopicInfo::ImplType_Name(topic_info.impl_type()).c_str(),
                     definition.c_str(), channel_source.DebugString().c_str())
              << std::endl;
  }
}

}  // namespace felicia