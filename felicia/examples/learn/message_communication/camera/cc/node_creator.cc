#include "felicia/core/felicia_init.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/examples/learn/message_communication/camera/cc/camera_flag.h"
#include "felicia/examples/learn/message_communication/camera/cc/camera_publishing_node.h"
#include "felicia/examples/learn/message_communication/camera/cc/camera_subscribing_node.h"

namespace felicia {

namespace {

void Print(const CameraDescriptors& camera_descriptors) {
  for (size_t i = 0; i < camera_descriptors.size(); ++i) {
    std::cout << "[" << i << "] " << camera_descriptors[i].ToString()
              << std::endl;
  }
}

void Print(const CameraFormats& camera_formats) {
  for (size_t i = 0; i < camera_formats.size(); ++i) {
    std::cout << "[" << i << "] " << camera_formats[i].ToString() << std::endl;
  }
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

  if (delegate.device_list_flag()->value()) {
    if (delegate.device_index_flag()->is_set()) {
      CameraFormats camera_formats;
      s = CameraFactory::GetSupportedCameraFormats(
          camera_descriptors[delegate.device_index_flag()->value()],
          &camera_formats);
      if (!s.ok()) {
        std::cerr << kRedError << s << std::endl;
        return 1;
      }
      Print(camera_formats);
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

  if (delegate.is_publishing_node_flag()->value()) {
    if (camera_descriptors.size() <= delegate.device_index_flag()->value()) {
      std::cerr << kRedError << "Please set device_index among them.."
                << std::endl;
      Print(camera_descriptors);
      return 1;
    }

    master_proxy.RequestRegisterNode<CameraPublishingNode>(
        node_info, delegate.topic_flag()->value(),
        delegate.channel_type_flag()->value(),
        camera_descriptors[delegate.device_index_flag()->value()],
        delegate.buffer_size_flag()->value());
  } else {
    master_proxy.RequestRegisterNode<CameraSubscribingNode>(
        node_info, delegate.topic_flag()->value(),
        delegate.buffer_size_flag()->value());
  }

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }
