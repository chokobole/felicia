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
constexpr size_t kChannelSourceLength = 22;

class ScopedMasterClientStopper {
 public:
  ScopedMasterClientStopper(GrpcMasterClient* client) : client_(client) {}
  ~ScopedMasterClientStopper() { client_->Stop(); }

 private:
  GrpcMasterClient* client_;
};

}  // namespace

CommandDispatcher::CommandDispatcher(std::unique_ptr<GrpcMasterClient> client)
    : master_client_(std::move(client)) {}

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
  master_client_->Start();

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
  master_client_->ListClientsAsync(
      request, response,
      ::base::BindOnce(&CommandDispatcher::OnListClientsAsync,
                       ::base::Unretained(this), master_client_.get(),
                       ::base::Owned(request), ::base::Owned(response)));
}

void CommandDispatcher::OnListClientsAsync(GrpcMasterClient* client,
                                           ListClientsRequest* request,
                                           ListClientsResponse* response,
                                           const Status& s) const {
  ScopedMasterClientStopper stopper(client);

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
    writer.SetElement(row, 1, ToString(hbs_channel_source));
    const ChannelSource& tiw_channel_source =
        client_info.topic_info_watcher_source();
    writer.SetElement(row, 2, ToString(tiw_channel_source));
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
  master_client_->Start();

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
  master_client_->ListNodesAsync(
      request, response,
      ::base::BindOnce(&CommandDispatcher::OnListNodesAsync,
                       ::base::Unretained(this), master_client_.get(),
                       ::base::Owned(request), ::base::Owned(response)));
}

void CommandDispatcher::OnListNodesAsync(GrpcMasterClient* client,
                                         ListNodesRequest* request,
                                         ListNodesResponse* response,
                                         const Status& s) const {
  ScopedMasterClientStopper stopper(client);

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
  }
}

void CommandDispatcher::Dispatch(const TopicListFlag& delegate) const {
  master_client_->Start();

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
  master_client_->ListTopicsAsync(
      request, response,
      ::base::BindOnce(&CommandDispatcher::OnListTopicsAsync,
                       ::base::Unretained(this), master_client_.get(),
                       ::base::Owned(request), ::base::Owned(response)));
}

void CommandDispatcher::OnListTopicsAsync(GrpcMasterClient* client,
                                          ListTopicsRequest* request,
                                          ListTopicsResponse* response,
                                          const Status& s) const {
  ScopedMasterClientStopper stopper(client);

  if (!s.ok()) {
    std::cerr << s.error_message() << std::endl;
    return;
  }
  auto topic_infos = response->topic_infos();
  TableWriterBuilder builder;
  auto writer =
      builder.AddColumn(TableWriter::Column{"NAME", kNameLength})
          .AddColumn(TableWriter::Column{"TYPE", 8})
          .AddColumn(TableWriter::Column{"END POINT", kChannelSourceLength})
          .Build();
  size_t row = 0;
  for (auto& topic_info : topic_infos) {
    writer.SetElement(row, 0, topic_info.topic());
    const ChannelSource& channel_source = topic_info.topic_source();
    writer.SetElement(row, 1, ToString(channel_source.channel_def()));
    writer.SetElement(row, 2, ToString(channel_source));
    row++;
  }

  std::cout << writer.ToString() << std::endl;
}

}  // namespace felicia