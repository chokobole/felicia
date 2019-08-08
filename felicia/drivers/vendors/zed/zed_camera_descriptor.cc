#include "felicia/drivers/vendors/zed/zed_camera_descriptor.h"

#include "third_party/chromium/base/strings/strcat.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"

namespace felicia {

int ZedCameraDescriptor::kInvalidId = -1;

ZedCameraDescriptor::ZedCameraDescriptor() : id_(-1) {}
ZedCameraDescriptor::ZedCameraDescriptor(
    const CameraDescriptor& camera_descriptor, int id)
    : CameraDescriptor(camera_descriptor), id_(id) {}
ZedCameraDescriptor::ZedCameraDescriptor(const std::string& display_name,
                                         const std::string& device_id,
                                         const std::string& model_id, int id)
    : CameraDescriptor(display_name, device_id, model_id), id_(id) {}

ZedCameraDescriptor::~ZedCameraDescriptor() = default;

ZedCameraDescriptor::ZedCameraDescriptor(const ZedCameraDescriptor& other) =
    default;
ZedCameraDescriptor& ZedCameraDescriptor::operator=(
    const ZedCameraDescriptor& other) = default;

int ZedCameraDescriptor::id() const { return id_; }

std::string ZedCameraDescriptor::ToString() const {
  return base::StrCat(
      {CameraDescriptor::ToString(), "id: ", base::NumberToString(id_)});
}

std::ostream& operator<<(std::ostream& os,
                         const ZedCameraDescriptor& camera_descriptor) {
  os << camera_descriptor.ToString();
  return os;
}

std::ostream& operator<<(std::ostream& os,
                         const ZedCameraDescriptors& camera_descriptors) {
  for (size_t i = 0; i < camera_descriptors.size(); ++i) {
    os << "[" << i << "] " << camera_descriptors[i] << std::endl;
  }
  return os;
}
}  // namespace felicia