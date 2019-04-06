#include "felicia/drivers/camera/camera_buffer.h"

namespace felicia {

CameraBuffer::CameraBuffer(uint8_t* start, size_t length)
    : start_(start), length_(length) {}
CameraBuffer::CameraBuffer(const CameraBuffer& other) = default;
CameraBuffer& CameraBuffer::operator=(const CameraBuffer& other) = default;
CameraBuffer::~CameraBuffer() = default;

uint8_t* CameraBuffer::start() const { return start_; }
size_t CameraBuffer::payload() const { return payload_; }
void CameraBuffer::set_payload(size_t payload) { payload_ = payload; }
size_t CameraBuffer::length() const { return length_; }

}  // namespace felicia
