#include "felicia/drivers/vendors/zed/zed_camera_frame.h"

namespace felicia {

CameraFrame ConverToCameraFrame(::sl::Mat image,
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

}  // namespace felicia