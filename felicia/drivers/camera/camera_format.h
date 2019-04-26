#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_

#include <string>

#include "third_party/chromium/build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/size.h"

namespace felicia {

class EXPORT CameraFormat {
 public:
  enum PixelFormat {
#define PIXEL_FORMAT(format) format,
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
  void set_pixel_format(PixelFormat pixel_format);
  int width() const;
  int height() const;
  void SetSize(int width, int height);
  float frame_rate() const;
  void set_frame_rate(float frame_rate);

#if defined(OS_LINUX)
  uint32_t ToV4l2PixelFormat() const;
  static PixelFormat FromV4l2PixelFormat(uint32_t v4l2_pixel_format);
#elif defined(OS_WIN)
  const GUID& ToMediaSubtype() const;
  static PixelFormat FromMediaSubtype(const GUID& sub_type);
#endif

 private:
  PixelFormat pixel_format_ = PIXEL_FORMAT_UNKNOWN;
  Sizei size_;
  float frame_rate_ = 0;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_