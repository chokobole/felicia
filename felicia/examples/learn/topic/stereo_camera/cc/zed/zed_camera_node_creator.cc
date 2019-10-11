#include "felicia/core/felicia_init.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/drivers/vendors/zed/zed_camera_factory.h"
#include "felicia/examples/learn/topic/stereo_camera/cc/stereo_camera_flag.h"
#include "felicia/examples/learn/topic/stereo_camera/cc/zed/zed_camera_publishing_node.h"

namespace felicia {

int RealMain(int argc, char* argv[]) {
  FeliciaInit();

  StereoCameraFlag delegate(1280, 760);
  FlagParser parser;
  parser.set_program_name("zed_camera_node_creator");
  if (!parser.Parse(argc, argv, &delegate)) {
    return 1;
  }

  drivers::ZedCameraDescriptors camera_descriptors;
  Status s =
      drivers::ZedCameraFactory::GetCameraDescriptors(&camera_descriptors);
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
      drivers::CameraFormats camera_formats;
      Status s = drivers::ZedCameraFactory::GetSupportedCameraFormats(
          camera_descriptors[delegate.device_index_flag()->value()],
          &camera_formats);
      if (!s.ok()) {
        std::cerr << kRedError << s << std::endl;
        return 1;
      }
      std::cout << camera_formats;
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

  master_proxy.RequestRegisterNode<ZedCameraPublishingNode>(
      node_info, delegate,
      camera_descriptors[delegate.device_index_flag()->value()]);

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }
