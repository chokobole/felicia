#include "felicia/core/felicia_init.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/drivers/camera/realsense/rs_camera_factory.h"
#include "felicia/examples/learn/message_communication/camera/cc/camera_flag.h"
#include "felicia/examples/learn/message_communication/camera/cc/camera_util.h"

namespace felicia {

namespace {

void Print(const RsCameraFormatMap& rs_camera_format_map) {
  for (auto& v : rs_camera_format_map) {
    std::cout << v.first.stream_type << "(" << v.first.stream_index << ")"
              << std::endl;
    Print(v.second);
  }
}

}  // namespace

int RealMain(int argc, char* argv[]) {
  FeliciaInit();

  CameraFlag delegate;
  FlagParser parser;
  parser.set_program_name("rs_node_creator");
  if (!parser.Parse(argc, argv, &delegate)) {
    return 1;
  }

  CameraDescriptors camera_descriptors;
  Status s = RsCameraFactory::GetCameraDescriptors(&camera_descriptors);
  if (!s.ok()) {
    std::cerr << kRedError << s << std::endl;
    return 1;
  }

  if (delegate.device_list_flag()->value()) {
    if (delegate.device_index_flag()->is_set()) {
      RsCameraFormatMap rs_camera_format_map;
      s = RsCameraFactory::GetSupportedCameraFormats(
          camera_descriptors[delegate.device_index_flag()->value()],
          &rs_camera_format_map);
      if (!s.ok()) {
        std::cerr << kRedError << s << std::endl;
        return 1;
      }
      Print(rs_camera_format_map);
    } else {
      Print(camera_descriptors);
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

  // if (delegate.is_publishing_node_flag()->value()) {
  //   if (camera_descriptors.size() <= delegate.device_index_flag()->value()) {
  //     std::cerr << kRedError << "Please set device_index among them.."
  //               << std::endl;
  //     Print(camera_descriptors);
  //     return 1;
  //   }

  //   master_proxy.RequestRegisterNode<CameraPublishingNode>(
  //       node_info, delegate.topic_flag()->value(),
  //       delegate.channel_type_flag()->value(),
  //       camera_descriptors[delegate.device_index_flag()->value()],
  //       delegate.buffer_size_flag()->value());
  // } else {
  //   master_proxy.RequestRegisterNode<CameraSubscribingNode>(
  //       node_info, delegate.topic_flag()->value(),
  //       delegate.buffer_size_flag()->value());
  // }

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }
