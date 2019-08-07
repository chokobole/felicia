#include "felicia/core/felicia_init.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/examples/slam/hecto_slam/hector_slam_node.h"
#include "felicia/examples/slam/slam_node_create_flag.h"

namespace felicia {

int RealMain(int argc, char* argv[]) {
  FeliciaInit();

  SlamNodeCreateFlag delegate;
  FlagParser parser;
  parser.set_program_name("slam_node_creator");
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
  if (delegate.slam_kind() == SlamNodeCreateFlag::SLAM_KIND_HECTOR_SLAM) {
    master_proxy.RequestRegisterNode<HectorSlamNode>(
        node_info, delegate.lidar_topic_flag()->value(),
        delegate.map_topic_flag()->value(), delegate.pose_topic_flag()->value(),
        delegate.hector_slam_delegate());
  }

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }