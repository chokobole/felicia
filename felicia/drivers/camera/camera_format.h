#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_

#include <vector>

#include "libyuv.h"
#include "third_party/chromium/build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_MACOSX)
typedef uint32_t FourCharCode;
#endif

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/size.h"
#include "felicia/drivers/camera/camera_format_message.pb.h"

namespace felicia {

const int kFrameRatePrecision = 10000;

class EXPORT CameraFormat {
 public:
  CameraFormat();
  CameraFormat(Sizei size, PixelFormat pixel_format, float frame_rate);
  CameraFormat(int width, int height, PixelFormat pixel_format,
               float frame_rate);
  CameraFormat(const CameraFormat& camera_format);
  CameraFormat& operator=(const CameraFormat& camera_format);

  static std::string FourccToString(uint32_t fourcc);

  std::string ToString() const;

  static const std::string& PixelFormatToString(PixelFormat pixel_format);

  PixelFormat pixel_format() const;
  void set_pixel_format(PixelFormat pixel_format);
  int width() const;
  int height() const;
  void SetSize(int width, int height);
  size_t AllocationSize() const;
  float frame_rate() const;
  void set_frame_rate(float frame_rate);

#if defined(OS_LINUX)
  uint32_t ToV4l2PixelFormat() const;
  static PixelFormat FromV4l2PixelFormat(uint32_t v4l2_pixel_format);
#elif defined(OS_WIN)
  const GUID& ToDshowMediaSubtype() const;
  static PixelFormat FromDshowMediaSubtype(const GUID& sub_type);
  static bool ToMfSinkMediaSubtype(const GUID& sub_type, GUID* sink_type);
  static PixelFormat FromMfMediaSubtype(const GUID& sub_type);
#elif defined(OS_MACOSX)
  FourCharCode ToAVFoundationPixelFormat() const;
  static PixelFormat FromAVFoundationPixelFormat(
      const FourCharCode avf_pixel_format);
#endif
  libyuv::FourCC ToLibyuvPixelFormat() const;

  CameraFormatMessage ToCameraFormatMessage() const;

  bool operator==(const CameraFormat& other);

 private:
  Sizei size_;
  PixelFormat pixel_format_;
  float frame_rate_ = 0;
};

using CameraFormats = std::vector<CameraFormat>;

// Prints a human-readable representation of |camera_format| to |os|.
EXPORT std::ostream& operator<<(std::ostream& os,
                                const CameraFormat& camera_format);

EXPORT std::ostream& operator<<(std::ostream& os,
                                const CameraFormats& camera_formats);

EXPORT bool ComparePixelFormatPreference(PixelFormat lhs, PixelFormat rhs);

// Compares the priority of the camera formats. Returns true if |lhs| is the
// preferred camera format in comparison with |rhs|. Returns false otherwise.
EXPORT bool CompareCapability(const CameraFormat& requested,
                              const CameraFormat& lhs, const CameraFormat& rhs);

EXPORT const CameraFormat& GetBestMatchedCameraFormat(
    const CameraFormat& requested, const CameraFormats& camera_formats);

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_