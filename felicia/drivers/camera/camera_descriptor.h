#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_DESCRIPTOR_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_DESCRIPTOR_H_

#include "felicia/core/lib/base/export.h"

#include <string>

namespace felicia {

class EXPORT CameraDescriptor {
 public:
  CameraDescriptor(const std::string& display_name,
                   const std::string& device_id);
  CameraDescriptor(const CameraDescriptor& other);
  CameraDescriptor& operator=(const CameraDescriptor& other);
  ~CameraDescriptor();

  const std::string& display_name() const;
  const std::string& device_id() const;

  std::string display_name_;
  std::string device_id_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_DESCRIPTOR_H_