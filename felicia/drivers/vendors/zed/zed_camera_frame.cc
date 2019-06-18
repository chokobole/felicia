#include "felicia/drivers/vendors/zed/zed_camera_frame.h"

#include "felicia/drivers/camera/camera_frame_util.h"

namespace felicia {

CameraFrame ConvertToCameraFrame(::sl::Mat image,
                                 const CameraFormat& camera_format) {
  size_t size = image.getStepBytes() * image.getHeight();
  std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
  ::sl::MAT_TYPE data_type = image.getDataType();

  switch (data_type) {
    case ::sl::MAT_TYPE_32F_C1:
      memcpy(data.get(), image.getPtr<::sl::float1>(), size);
      break;
    case ::sl::MAT_TYPE_32F_C2:
      memcpy(data.get(), image.getPtr<::sl::float2>(), size);
      break;
    case ::sl::MAT_TYPE_32F_C3:
      memcpy(data.get(), image.getPtr<::sl::float3>(), size);
      break;
    case ::sl::MAT_TYPE_32F_C4:
      memcpy(data.get(), image.getPtr<::sl::float4>(), size);
      break;
    case ::sl::MAT_TYPE_8U_C1:
      memcpy(data.get(), image.getPtr<::sl::uchar1>(), size);
      break;
    case ::sl::MAT_TYPE_8U_C2:
      memcpy(data.get(), image.getPtr<::sl::uchar2>(), size);
      break;
    case ::sl::MAT_TYPE_8U_C3:
      memcpy(data.get(), image.getPtr<::sl::uchar3>(), size);
      break;
    case ::sl::MAT_TYPE_8U_C4:
      memcpy(data.get(), image.getPtr<::sl::uchar4>(), size);
      break;
  }

  return CameraFrame(std::move(data), size, camera_format);
}

DepthCameraFrame ConvertToDepthCameraFrame(::sl::Mat image,
                                           const CameraFormat& camera_format,
                                           float min, float max) {
  size_t size = image.getWidth() * image.getHeight();
  size_t allocation_size = 2 * size;
  std::unique_ptr<uint8_t[]> data(new uint8_t[allocation_size]);
  ::sl::float1* image_ptr = image.getPtr<::sl::float1>();
  for (size_t i = 0; i < size; ++i) {
    const size_t data_idx = i << 1;
    uint16_t value = static_cast<uint16_t>(*(image_ptr++) * 1000);
    data[data_idx] = static_cast<uint8_t>(value & UINT8_MAX);
    data[data_idx + 1] = static_cast<uint8_t>(value >> 8);
  }
  CameraFrame frame(std::move(data), allocation_size, camera_format);
  return DepthCameraFrame(std::move(frame), min * 1000, max * 1000);
}

}  // namespace felicia