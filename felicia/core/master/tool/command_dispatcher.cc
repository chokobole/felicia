#include "felicia/core/master/tool/command_dispatcher.h"

#include <iostream>
#include <utility>

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/net/base/ip_endpoint.h"

#include "felicia/core/master/master_data.pb.h"
#include "felicia/core/master/rpc/scoped_grpc_request.h"
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
      request, response, [this, request, response](const Status& s) {
        ScopedOneTimeGrpcRequest<GetNodesRequest, GetNodesResponse>
            scoped_request({master_client_.get(), request, response});
        if (!s.ok()) {
          std::cerr << s.error_message() << std::endl;
          return;
        }
        auto node_infos = response->node_infos();

        TableWriterBuilder builder;
        auto writer = builder.AddColumn(TableWriter::Column{"NAME", 20})
                          .AddColumn(TableWriter::Column{"IP ENDPOINT", 32})
                          .Build();
        size_t row = 0;
        for (auto& node_info : node_infos) {
          writer.SetElement(row, 0, node_info.name());
          ::net::IPAddress ip;
          ip.AssignFromIPLiteral(node_info.ip_endpoint().ip());
          uint16_t port = node_info.ip_endpoint().port();
          writer.SetElement(row, 1, ::net::IPEndPoint{ip, port}.ToString());
          row++;
        }

        std::cout << writer.ToString() << std::endl;
      });

  master_client_->Join();
}

}  // namespace felicia