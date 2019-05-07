#include "felicia/examples/learn/message_communication/camera/cc/camera_util.h"

namespace felicia {

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

}  // namespace felicia