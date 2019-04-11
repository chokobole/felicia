#include "grpcpp/grpcpp.h"

#include "felicia/core/felicia_init.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/core/master/rpc/grpc_master_client.h"
#include "felicia/core/master/rpc/grpc_util.h"
#include "felicia/core/master/tool/cli_flag.h"
#include "felicia/core/master/tool/command_dispatcher.h"

namespace felicia {

int RealMain(int argc, char* argv[]) {
  FeliciaInit();

  CliFlag delegate;
  FlagParser parser;
  parser.set_program_name(argv[0]);
  if (!parser.Parse(argc, argv, &delegate)) {
    return 1;
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  Status s = master_proxy.Start();
  CHECK(s.ok());

  CommandDispatcher dispatcher;
  dispatcher.Dispatch(delegate);

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }