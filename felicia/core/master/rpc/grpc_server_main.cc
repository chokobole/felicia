#include "felicia/core/felicia_init.h"
#include "felicia/core/master/rpc/grpc_server.h"

namespace felicia {

int RealMain(int argc, char* argv[]) {
  FeliciaInit();

  GrpcServer server;
  server.Init();
  server.Start();
  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }