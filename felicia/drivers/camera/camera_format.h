#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_

#include <string>
#include <vector>

#include "third_party/chromium/build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_MACOSX)
typedef uint32_t FourCharCode;
#endif

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/size.h"

namespace felicia {

const int kFrameRatePrecision = 10000;

class EXPORT CameraFormat {
 public:
  enum PixelFormat {
#define PIXEL_FORMAT(format) format,
#include "felicia/drivers/camera/camera_format_list.h"
#undef PIXEL_FORMAT
  };

  CameraFormat();
  CameraFormat(Sizei size, PixelFormat pixel_format, float frame_rate);
  CameraFormat(int width, int height, PixelFormat pixel_format,
               float frame_rate);

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
#elif defined(OS_MACOSX)
  FourCharCode ToAVFoundationPixelFormat() const;
  static PixelFormat FromAVFoundationPixelFormat(
      const FourCharCode avf_pixel_format);
#endif

 private:
  Sizei size_;
  PixelFormat pixel_format_ = PIXEL_FORMAT_UNKNOWN;
  float frame_rate_ = 0;
};

using CameraFormats = std::vector<CameraFormat>;

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_