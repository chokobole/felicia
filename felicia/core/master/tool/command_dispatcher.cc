#include "felicia/core/master/tool/command_dispatcher.h"

#include <iostream>
#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/util/command_line_interface/table_writer.h"

namespace felicia {

CommandDispatcher::CommandDispatcher(std::unique_ptr<GrpcMasterClient> client)
    : master_client_(std::move(client)) {}

void CommandDispatcher::Dispatch(const FlagParserDelegate& delegate) const {
  auto command = delegate.command();
  switch (command) {
    case FlagParserDelegate::Command::COMMAND_SELF:
      break;
    case FlagParserDelegate::Command::COMMAND_NODE:
      Dispatch(delegate.node_delegate());
      break;
    case FlagParserDelegate::Command::COMMAND_TOPIC:
      Dispatch(delegate.topic_delegate());
      break;
  }
}

void CommandDispatcher::Dispatch(const NodeFlagParserDelegate& delegate) const {
  auto command = delegate.command();
  switch (command) {
    case NodeFlagParserDelegate::Command::COMMAND_SELF:
      NOTREACHED();
      break;
    case NodeFlagParserDelegate::Command::COMMAND_LIST:
      Dispatch(delegate.list_delegate());
      break;
  }
}

void CommandDispatcher::Dispatch(
    const NodeListFlagParserDelegate& delegate) const {
  master_client_->Start();

  NodeFilter node_filter;
  if (delegate.all()) {
    node_filter.set_all(true);
  } else if (!delegate.publishing_topic().empty()) {
    node_filter.set_publishing_topic(delegate.publishing_topic());
  } else if (!delegate.subscribing_topic().empty()) {
    node_filter.set_subscribing_topic(delegate.subscribing_topic());
  } else {
    DCHECK(!delegate.name().empty());
    node_filter.set_name(delegate.name());
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
  if (!s.ok()) {
    std::cerr << s.error_message() << std::endl;
    client->Shutdown();
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
    auto writer = builder.AddColumn(TableWriter::Column{"NAME", 20})
                      .AddColumn(TableWriter::Column{"CLIENT ID", 10})
                      .Build();
    size_t row = 0;
    for (auto& node_info : node_infos) {
      writer.SetElement(row, 0, node_info.name());
      writer.SetElement(row, 1, ::base::NumberToString(node_info.client_id()));
      row++;
    }

    std::cout << writer.ToString() << std::endl;
  }
  client->Shutdown();
}

void CommandDispatcher::Dispatch(
    const TopicFlagParserDelegate& delegate) const {
  auto command = delegate.command();
  switch (command) {
    case TopicFlagParserDelegate::Command::COMMAND_SELF:
      NOTREACHED();
      break;
    case TopicFlagParserDelegate::Command::COMMAND_LIST:
      Dispatch(delegate.list_delegate());
      break;
  }
}

void CommandDispatcher::Dispatch(
    const TopicListFlagParserDelegate& delegate) const {
  master_client_->Start();

  TopicFilter topic_filter;
  if (delegate.all()) {
    topic_filter.set_all(true);
  } else {
    DCHECK(!delegate.topic().empty());
    topic_filter.set_topic(delegate.topic());
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
  if (!s.ok()) {
    std::cerr << s.error_message() << std::endl;
    client->Shutdown();
    return;
  }
  auto topic_infos = response->topic_infos();
  TableWriterBuilder builder;
  auto writer = builder.AddColumn(TableWriter::Column{"NAME", 20})
                    .AddColumn(TableWriter::Column{"TYPE", 8})
                    .AddColumn(TableWriter::Column{"END POINT", 10})
                    .Build();
  size_t row = 0;
  for (auto& topic_info : topic_infos) {
    writer.SetElement(row, 0, topic_info.topic());
    const ChannelSource& channel_source = topic_info.topic_source();
    writer.SetElement(row, 1, ToString(channel_source.channel_def()));
    if (channel_source.has_ip_endpoint()) {
      ::net::IPEndPoint ip_endpoint;
      if (ToNetIPEndPoint(channel_source, &ip_endpoint)) {
        writer.SetElement(row, 2, ip_endpoint.ToString());
      }
    }
    row++;
  }

  std::cout << writer.ToString() << std::endl;
  client->Shutdown();
}

}  // namespace felicia