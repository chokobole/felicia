// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

#if defined(HAS_OPENCV)
#include <opencv2/core.hpp>
#endif  // defined(HAS_OPENCV)
#if defined(HAS_ROS)
#include <sensor_msgs/image_encodings.h>
#endif  // defined(HAS_ROS)

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/lib/unit/geometry/size.h"
#include "felicia/drivers/camera/camera_format_message.pb.h"

namespace felicia {
namespace drivers {

const int kFrameRatePrecision = 10000;

class FEL_EXPORT CameraFormat {
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

  bool HasFixedSizedChannelPixelFormat() const;

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

#if defined(HAS_OPENCV)
  int ToCvType() const;
#endif  // defined(HAS_OPENCV)
#if defined(HAS_ROS)
  // Return appropriate string value defined in "sensor_msgs/image_encodings.h",
  // return empty string if failed to convert.
  std::string ToRosImageEncoding() const;
  static PixelFormat FromRosImageEncoding(const std::string& ros_encoding);
#endif  // defined(HAS_ROS)

  CameraFormatMessage ToCameraFormatMessage() const;
  Status FromCameraFormatMessage(const CameraFormatMessage& message);

  bool operator==(const CameraFormat& other);

 private:
  Sizei size_;
  PixelFormat pixel_format_;
  float frame_rate_ = 0;
};

using CameraFormats = std::vector<CameraFormat>;

// Prints a human-readable representation of |camera_format| to |os|.
FEL_EXPORT std::ostream& operator<<(std::ostream& os,
                                    const CameraFormat& camera_format);

FEL_EXPORT std::ostream& operator<<(std::ostream& os,
                                    const CameraFormats& camera_formats);

FEL_EXPORT bool ComparePixelFormatPreference(PixelFormat lhs, PixelFormat rhs);

// Compares the priority of the camera formats. Returns true if |lhs| is the
// preferred camera format in comparison with |rhs|. Returns false otherwise.
FEL_EXPORT bool CompareCapability(const CameraFormat& requested,
                                  const CameraFormat& lhs,
                                  const CameraFormat& rhs);

FEL_EXPORT const CameraFormat& GetBestMatchedCameraFormat(
    const CameraFormat& requested, const CameraFormats& camera_formats);

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FORMAT_H_