#ifndef FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_DESCRIPTOR_H_
#define FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_DESCRIPTOR_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/drivers/camera/camera_descriptor.h"

namespace felicia {
namespace drivers {

class EXPORT ZedCameraDescriptor : public CameraDescriptor {
 public:
  static int kInvalidId;

  ZedCameraDescriptor();
  ZedCameraDescriptor(const CameraDescriptor& camera_descriptor, int id);
  ZedCameraDescriptor(const std::string& display_name,
                      const std::string& device_id, const std::string& model_id,
                      int id);
  ZedCameraDescriptor(const ZedCameraDescriptor& other);
  ZedCameraDescriptor& operator=(const ZedCameraDescriptor& other);
  ~ZedCameraDescriptor();

  int id() const;

  std::string ToString() const;

 private:
  int id_;
};

using ZedCameraDescriptors = std::vector<ZedCameraDescriptor>;

EXPORT std::ostream& operator<<(std::ostream& os,
                                const ZedCameraDescriptor& camera_descriptor);

EXPORT std::ostream& operator<<(std::ostream& os,
                                const ZedCameraDescriptors& camera_descriptors);

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_DESCRIPTOR_H_