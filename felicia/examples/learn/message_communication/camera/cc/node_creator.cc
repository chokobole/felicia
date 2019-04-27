#include "felicia/core/felicia_init.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/examples/learn/message_communication/camera/cc/camera_flag.h"
#include "felicia/examples/learn/message_communication/camera/cc/camera_publishing_node.h"
#include "felicia/examples/learn/message_communication/camera/cc/camera_subscribing_node.h"

namespace felicia {

int RealMain(int argc, char* argv[]) {
  FeliciaInit();

  CameraFlag delegate;
  FlagParser parser;
  parser.set_program_name("node_creator");
  if (!parser.Parse(argc, argv, &delegate)) {
    return 1;
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  Status s = master_proxy.Start();
  CHECK(s.ok());

  NodeInfo node_info;
  node_info.set_name(delegate.name());

  if (delegate.is_publishing_node()) {
    master_proxy.RequestRegisterNode<CameraPublishingNode>(
        node_info, delegate.topic(), delegate.channel_type(),
        delegate.device_id(), delegate.buffer_size());
  } else {
    master_proxy.RequestRegisterNode<CameraSubscribingNode>(
        node_info, delegate.topic(), delegate.buffer_size());
  }

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }
