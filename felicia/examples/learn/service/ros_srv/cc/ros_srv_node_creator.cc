#include "felicia/core/felicia_init.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/examples/learn/service/common/cc/simple_service_flag.h"
#include "felicia/examples/learn/service/ros_srv/cc/ros_srv_client_node.h"
#include "felicia/examples/learn/service/ros_srv/cc/ros_srv_server_node.h"

namespace felicia {

int RealMain(int argc, char* argv[]) {
  FeliciaInit();

  SimpleServiceFlag delegate;
  FlagParser parser;
  parser.set_program_name("ros_srv_node_creator");
  if (!parser.Parse(argc, argv, &delegate)) {
    return 1;
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  Status s = master_proxy.Start();
  if (!s.ok()) {
    std::cerr << kRedError << s << std::endl;
    return 1;
  }

  NodeInfo node_info;
  node_info.set_name(delegate.name_flag()->value());

  if (delegate.is_server_flag()->value()) {
    master_proxy.RequestRegisterNode<RosSrvServerNode>(node_info, delegate);
  } else {
    master_proxy.RequestRegisterNode<RosSrvClientNode>(node_info, delegate);
  }

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }