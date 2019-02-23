#include "grpcpp/grpcpp.h"

#include "felicia/core/master/client/grpc_master_client.h"
#include "felicia/core/master/rpc/grpc_util.h"
#include "felicia/core/master/tool/command_dispatcher.h"
#include "felicia/core/master/tool/flag_parser_delegate.h"

namespace felicia {

int RealMain(int argc, char* argv[]) {
  FlagParserDelegate delegate;
  FlagParser parser;
  parser.set_program_name(argv[0]);
  if (!parser.Parse(argc, argv, &delegate)) {
    return 1;
  }

  auto channel = ConnectGRPCService();

  std::unique_ptr<GrpcMasterClient> client =
      std::make_unique<GrpcMasterClient>(channel);
  CommandDispatcher dispatcher(std::move(client));
  dispatcher.Dispatch(delegate);

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }