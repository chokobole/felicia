#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_BUFFER_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_BUFFER_H_

#include <stddef.h>
#include <stdint.h>

#include <vector>

#include "felicia/core/lib/base/export.h"

namespace felicia {
namespace drivers {

class EXPORT CameraBuffer {
 public:
  CameraBuffer(uint8_t* start, size_t length);
  CameraBuffer(const CameraBuffer& other);
  CameraBuffer& operator=(const CameraBuffer& other);
  ~CameraBuffer();

  uint8_t* start() const;
  size_t payload() const;
  void set_payload(size_t payload);
  size_t length() const;

 private:
  uint8_t* start_;
  size_t payload_;
  size_t length_;
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_BUFFER_H_