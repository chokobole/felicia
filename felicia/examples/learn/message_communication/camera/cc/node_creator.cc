#include "felicia/core/felicia_init.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/examples/learn/message_communication/camera/cc/camera_flag.h"
#include "felicia/examples/learn/message_communication/camera/cc/camera_publishing_node.h"
#include "felicia/examples/learn/message_communication/camera/cc/camera_subscribing_node.h"

namespace felicia {

namespace {

void Print(const CameraDescriptors& camera_descriptors) {
  std::stringstream ss;
  for (size_t i = 0; i < camera_descriptors.size(); ++i) {
    ss << "[" << i << "] " << camera_descriptors[i].ToString() << std::endl;
  }
  std::cout << ss.str() << std::endl;
}

}  // namespace

int RealMain(int argc, char* argv[]) {
  FeliciaInit();

  CameraFlag delegate;
  FlagParser parser;
  parser.set_program_name("node_creator");
  if (!parser.Parse(argc, argv, &delegate)) {
    return 1;
  }

  CameraDescriptors camera_descriptors;
  Status s = CameraFactory::GetCameraDescriptors(&camera_descriptors);
  if (!s.ok()) {
    std::cerr << kRedError << s << std::endl;
    return 1;
  }

  if (delegate.device_list()) {
    Print(camera_descriptors);
    return 0;
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  s = master_proxy.Start();
  if (!s.ok()) {
    std::cerr << kRedError << s << std::endl;
    return 1;
  }

  NodeInfo node_info;
  node_info.set_name(delegate.name());

  if (delegate.is_publishing_node()) {
    if (camera_descriptors.size() <= delegate.device_index()) {
      std::cerr << kRedError << "Please set device_index among them.."
                << std::endl;
      Print(camera_descriptors);
      return 1;
    }

    master_proxy.RequestRegisterNode<CameraPublishingNode>(
        node_info, delegate.topic(), delegate.channel_type(),
        camera_descriptors[delegate.device_index()], delegate.buffer_size());
  } else {
    master_proxy.RequestRegisterNode<CameraSubscribingNode>(
        node_info, delegate.topic(), delegate.buffer_size());
  }

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }
