#include "felicia/core/felicia_init.h"
#include "felicia/core/master/rpc/master_server.h"
#if defined(HAS_ROS)
#include "felicia/core/master/ros_master_proxy.h"
#endif  // defined(HAS_ROS)

namespace felicia {

namespace {

#if defined(HAS_ROS)
void ShutdownROSMasterProxy() {
  ROSMasterProxy& ros_master_proxy = ROSMasterProxy::GetInstance();
  ros_master_proxy.Shutdown();
}
#endif  // defined(HAS_ROS)

}  // namespace

int RealMain(int argc, char* argv[]) {
  FeliciaInit();

  MasterServer server;
  Status s = server.Start();
  CHECK(s.ok()) << s;
#if defined(HAS_ROS)
  ROSMasterProxy& ros_master_proxy = ROSMasterProxy::GetInstance();
  s = ros_master_proxy.Init(server.master());
  CHECK(s.ok()) << s;
  s = ros_master_proxy.Start();
  CHECK(s.ok()) << s;
  server.set_on_shutdown_callback(base::BindOnce(&ShutdownROSMasterProxy));
#endif  // defined(HAS_ROS)
  server.RunUntilShutdown();
  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }