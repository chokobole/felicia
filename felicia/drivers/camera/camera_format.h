#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_

#include <string>

#include "third_party/chromium/build/build_config.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/size.h"

namespace felicia {

class EXPORT CameraFormat {
 public:
  enum PixelFormat {
#define PIXEL_FORMAT(format, v4l2_format) format,
#include "felicia/drivers/camera/camera_format_list.h"
    PIXEL_FORMAT_UNKNOWN,
#undef PIXEL_FORMAT
  };

  CameraFormat();
  CameraFormat(int width, int height, PixelFormat pixel_format);

  static std::string FourccToString(uint32_t fourcc);

  std::string ToString() const;

  static std::string PixelFormatToString(PixelFormat pixel_format);

  PixelFormat pixel_format() const;
  int width() const;
  int height() const;

#if defined(OS_LINUX)
  uint32_t ToV4l2PixelFormat() const;
  static PixelFormat FromV4l2PixelFormat(uint32_t fourcc);
#endif

 private:
  PixelFormat pixel_format_;
  Sizei size_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_