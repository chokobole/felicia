#include "felicia/core/felicia_init.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/examples/learn/message_communication/depth_camera/cc/realsense/rs_camera_flag.h"
#include "felicia/examples/learn/message_communication/depth_camera/cc/realsense/rs_camera_publishing_node.h"

namespace felicia {

namespace {

void Print(const RsCapabilityList& rs_capabilities) {
  size_t i = 0;
  for (auto& rs_capability : rs_capabilities) {
    std::cout << "[" << i << "] ";
    if (rs_capability.is_camera_format)
      std::cout << rs_capability.format.camera_format;
    else
      std::cout << rs_capability.format.imu_format;
    std::cout << std::endl;
    i++;
  }
}

void Print(const RsCapabilityMap& rs_capability_map) {
  for (auto& v : rs_capability_map) {
    std::cout << v.first.stream_type << "(" << v.first.stream_index << ")"
              << std::endl;
    Print(v.second);
  }
}

}  // namespace

int RealMain(int argc, char* argv[]) {
  FeliciaInit();

  RsCameraFlag delegate;
  FlagParser parser;
  parser.set_program_name("rs_camera_node_creator");
  if (!parser.Parse(argc, argv, &delegate)) {
    return 1;
  }

  CameraDescriptors camera_descriptors;
  Status s = RsCameraFactory::GetCameraDescriptors(&camera_descriptors);
  if (!s.ok()) {
    std::cerr << kRedError << s << std::endl;
    return 1;
  }

  if (delegate.device_index_flag()->is_set()) {
    if (camera_descriptors.size() <= delegate.device_index_flag()->value()) {
      std::cerr << kRedError << "Please set device_index among them.."
                << std::endl;
      std::cout << camera_descriptors;
      return 1;
    }
  }

  if (delegate.device_list_flag()->value()) {
    if (delegate.device_index_flag()->is_set()) {
      RsCapabilityMap rs_capability_map;
      Status s = RsCameraFactory::GetSupportedCapabilities(
          camera_descriptors[delegate.device_index_flag()->value()],
          &rs_capability_map);
      if (!s.ok()) {
        std::cerr << kRedError << s << std::endl;
        return 1;
      }
      Print(rs_capability_map);
    } else {
      std::cout << camera_descriptors;
    }
    return 0;
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  s = master_proxy.Start();
  if (!s.ok()) {
    std::cerr << kRedError << s << std::endl;
    return 1;
  }

  NodeInfo node_info;
  node_info.set_name(delegate.name_flag()->value());

  master_proxy.RequestRegisterNode<RsCameraPublishingNode>(
      node_info, delegate.color_topic_flag()->value(),
      delegate.depth_topic_flag()->value(), delegate.imu_topic_flag()->value(),
      delegate.synched_flag()->value(),
      camera_descriptors[delegate.device_index_flag()->value()]);

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }
