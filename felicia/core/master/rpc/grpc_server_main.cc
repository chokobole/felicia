#include "felicia/core/master/rpc/grpc_server.h"

namespace felicia {

int RealMain(int argc, char* argv[]) {
  GrpcServer server;
  server.Init();
  server.Start();
  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }