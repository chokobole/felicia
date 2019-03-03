#include "felicia/core/master/tool/command_dispatcher.h"

#include <iostream>
#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/master/master_data.pb.h"
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
  }
}

void CommandDispatcher::Dispatch(const NodeFlagParserDelegate& delegate) const {
  auto command = delegate.command();
  switch (command) {
    case NodeFlagParserDelegate::Command::COMMAND_SELF:
      NOTREACHED();
      break;
    case NodeFlagParserDelegate::Command::COMMAND_CREATE:
      Dispatch(delegate.create_delegate());
      break;
    case NodeFlagParserDelegate::Command::COMMAND_GET:
      Dispatch(delegate.get_delegate());
      break;
  }
}

void CommandDispatcher::Dispatch(
    const NodeCreateFlagParserDelegate& delegate) const {}

void CommandDispatcher::Dispatch(
    const NodeGetFlagParserDelegate& delegate) const {
  master_client_->Start();

  NodeFilter node_filter;
  if (delegate.all()) {
    node_filter.set_all(true);
  } else if (!delegate.publishing_topic().empty()) {
    node_filter.set_publishing_topic(std::string(delegate.publishing_topic()));
  } else {
    DCHECK(!delegate.subscribing_topic().empty());
    node_filter.set_subscribing_topic(
        std::string(delegate.subscribing_topic()));
  }
  GetNodesRequest* request = new GetNodesRequest();
  *request->mutable_node_filter() = node_filter;
  GetNodesResponse* response = new GetNodesResponse();
  master_client_->GetNodesAsync(
      request, response,
      ::base::BindOnce(&CommandDispatcher::OnGetNodesAsync,
                       ::base::Unretained(this), master_client_.get(),
                       ::base::Owned(request), ::base::Owned(response)));

  master_client_->Join();
}

void CommandDispatcher::OnGetNodesAsync(GrpcMasterClient* client,
                                        GetNodesRequest* request,
                                        GetNodesResponse* response,
                                        const Status& s) const {
  if (!s.ok()) {
    std::cerr << s.error_message() << std::endl;
    client->Shutdown();
    return;
  }
  auto node_infos = response->node_infos();
  TableWriterBuilder builder;
  auto writer = builder.AddColumn(TableWriter::Column{"NAME", 20})
                    .AddColumn(TableWriter::Column{"Client ID", 10})
                    .Build();
  size_t row = 0;
  for (auto& node_info : node_infos) {
    writer.SetElement(row, 0, node_info.name());
    writer.SetElement(row, 1, ::base::NumberToString(node_info.client_id()));
    row++;
  }

  std::cout << writer.ToString() << std::endl;
  client->Shutdown();
}

}  // namespace felicia