#ifndef FELICIA_DRIVERS_CAMERA_LINUX_CAMERA_BUFFER_H_
#define FELICIA_DRIVERS_CAMERA_LINUX_CAMERA_BUFFER_H_

#include <vector>

namespace felicia {

class CameraBuffer {
 public:
  CameraBuffer(uint8_t* start, size_t length)
      : start_(start), length_(length) {}
  CameraBuffer(const CameraBuffer& other) = default;
  CameraBuffer& operator=(const CameraBuffer& other) = default;
  ~CameraBuffer() = default;

  uint8_t* start() const { return start_; }
  size_t payload() const { return payload_; }
  void set_payload(size_t payload) { payload_ = payload; }
  size_t length() const { return length_; }

 private:
  uint8_t* start_;
  size_t payload_;
  size_t length_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_LINUX_CAMERA_BUFFER_H_