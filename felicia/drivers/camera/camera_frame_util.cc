// Followings are taken and modified from chromium/media/base/video_frames.cc
// - bool RequiresEvenSizeAllocation(CameraFormat camera_format);
// - size_t AllocationSize(CameraFormat camera_format);
// - bool IsValidPlane(size_t plane, CameraFormat camera_format);
// - Sizei SampleSize(CameraFormat camera_format, size_t plane);
// - int BytesPerElement(CameraFormat camera_format, size_t plane);
// - Sizei PlaneSize(CameraFormat camera_format, size_t plane);
// Followings are taken and modified from
// chromium/media/base/video_frame_layout.cc
// - size_t NumPlanes(CameraFormat camera_format);

#include "felicia/drivers/camera/camera_frame_util.h"

#include "third_party/chromium/base/bits.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/stl_util.h"

namespace felicia {

namespace {

enum {
  kMaxPlanes = 4,

  kYPlane = 0,
  kARGBPlane = kYPlane,
  kUPlane = 1,
  kUVPlane = kUPlane,
  kVPlane = 2,
  kAPlane = 3,
};

// If it is required to allocate aligned to multiple-of-two size overall for the
// frame of pixel |camera_format|.
bool RequiresEvenSizeAllocation(CameraFormat camera_format) {
  CameraFormat::PixelFormat pixel_format = camera_format.pixel_format();
  switch (pixel_format) {
    case CameraFormat::PIXEL_FORMAT_ARGB:
    // case CameraFormat::PIXEL_FORMAT_XRGB:
    case CameraFormat::PIXEL_FORMAT_RGB24:
    case CameraFormat::PIXEL_FORMAT_RGB32:
      // case CameraFormat::PIXEL_FORMAT_Y16:
      // case CameraFormat::PIXEL_FORMAT_ABGR:
      // case CameraFormat::PIXEL_FORMAT_XBGR:
      return false;
    case CameraFormat::PIXEL_FORMAT_NV12:
    case CameraFormat::PIXEL_FORMAT_NV21:
    // case CameraFormat::PIXEL_FORMAT_MT21:
    case CameraFormat::PIXEL_FORMAT_I420:
    case CameraFormat::PIXEL_FORMAT_MJPEG:
    case CameraFormat::PIXEL_FORMAT_YUY2:
    case CameraFormat::PIXEL_FORMAT_YV12:
    // case CameraFormat::PIXEL_FORMAT_I422:
    // case CameraFormat::PIXEL_FORMAT_I444:
    // case CameraFormat::PIXEL_FORMAT_YUV420P9:
    // case CameraFormat::PIXEL_FORMAT_YUV422P9:
    // case CameraFormat::PIXEL_FORMAT_YUV444P9:
    // case CameraFormat::PIXEL_FORMAT_YUV420P10:
    // case CameraFormat::PIXEL_FORMAT_YUV422P10:
    // case CameraFormat::PIXEL_FORMAT_YUV444P10:
    // case CameraFormat::PIXEL_FORMAT_YUV420P12:
    // case CameraFormat::PIXEL_FORMAT_YUV422P12:
    // case CameraFormat::PIXEL_FORMAT_YUV444P12:
    // case CameraFormat::PIXEL_FORMAT_I420A:
    case CameraFormat::PIXEL_FORMAT_UYVY:
      // case CameraFormat::PIXEL_FORMAT_P016LE:
      return true;
    case CameraFormat::PIXEL_FORMAT_UNKNOWN:
      break;
  }
  NOTREACHED() << "Unsupported video frame format: " << pixel_format;
  return false;
}

// static
size_t NumPlanes(CameraFormat camera_format) {
  CameraFormat::PixelFormat pixel_format = camera_format.pixel_format();
  switch (pixel_format) {
    case CameraFormat::PIXEL_FORMAT_UYVY:
    case CameraFormat::PIXEL_FORMAT_YUY2:
    case CameraFormat::PIXEL_FORMAT_ARGB:
    // case CameraFormat::PIXEL_FORMAT_XRGB:
    case CameraFormat::PIXEL_FORMAT_RGB24:
    case CameraFormat::PIXEL_FORMAT_RGB32:
    case CameraFormat::PIXEL_FORMAT_MJPEG:
      // case CameraFormat::PIXEL_FORMAT_Y16:
      // case CameraFormat::PIXEL_FORMAT_ABGR:
      // case CameraFormat::PIXEL_FORMAT_XBGR:
      return 1;
    case CameraFormat::PIXEL_FORMAT_NV12:
    case CameraFormat::PIXEL_FORMAT_NV21:
      // case CameraFormat::PIXEL_FORMAT_MT21:
      // case CameraFormat::PIXEL_FORMAT_P016LE:
      return 2;
    case CameraFormat::PIXEL_FORMAT_I420:
    case CameraFormat::PIXEL_FORMAT_YV12:
      // case CameraFormat::PIXEL_FORMAT_I422:
      // case CameraFormat::PIXEL_FORMAT_I444:
      // case CameraFormat::PIXEL_FORMAT_YUV420P9:
      // case CameraFormat::PIXEL_FORMAT_YUV422P9:
      // case CameraFormat::PIXEL_FORMAT_YUV444P9:
      // case CameraFormat::PIXEL_FORMAT_YUV420P10:
      // case CameraFormat::PIXEL_FORMAT_YUV422P10:
      // case CameraFormat::PIXEL_FORMAT_YUV444P10:
      // case CameraFormat::PIXEL_FORMAT_YUV420P12:
      // case CameraFormat::PIXEL_FORMAT_YUV422P12:
      // case CameraFormat::PIXEL_FORMAT_YUV444P12:
      return 3;
    // case CameraFormat::PIXEL_FORMAT_I420A:
    //   return 4;
    case CameraFormat::PIXEL_FORMAT_UNKNOWN:
      // Note: PIXEL_FORMAT_UNKNOWN is used for end-of-stream frame.
      // Set its NumPlanes() to zero to avoid NOTREACHED().
      return 0;
  }
  NOTREACHED() << "Unsupported video frame format: " << pixel_format;
  return 0;
}

bool IsValidPlane(size_t plane, CameraFormat camera_format) {
  DCHECK_LE(NumPlanes(camera_format), static_cast<size_t>(kMaxPlanes));
  return (plane < NumPlanes(camera_format));
}

Sizei SampleSize(CameraFormat camera_format, size_t plane) {
  DCHECK(IsValidPlane(plane, camera_format));
  CameraFormat::PixelFormat pixel_format = camera_format.pixel_format();

  switch (plane) {
    case kYPlane:  // and kARGBPlane:
    case kAPlane:
      return Sizei(1, 1);

    case kUPlane:  // and kUVPlane:
    case kVPlane:
      switch (pixel_format) {
          // case CameraFormat::PIXEL_FORMAT_I444:
          // case CameraFormat::PIXEL_FORMAT_YUV444P9:
          // case CameraFormat::PIXEL_FORMAT_YUV444P10:
          // case CameraFormat::PIXEL_FORMAT_YUV444P12:
          // case CameraFormat::PIXEL_FORMAT_Y16:
          //   return gfx::Size(1, 1);

          // case CameraFormat::PIXEL_FORMAT_I422:
          // case CameraFormat::PIXEL_FORMAT_YUV422P9:
          // case CameraFormat::PIXEL_FORMAT_YUV422P10:
          // case CameraFormat::PIXEL_FORMAT_YUV422P12:
          //   return gfx::Size(2, 1);

        case CameraFormat::PIXEL_FORMAT_YV12:
        case CameraFormat::PIXEL_FORMAT_I420:
        // case CameraFormat::PIXEL_FORMAT_I420A:
        case CameraFormat::PIXEL_FORMAT_NV12:
        case CameraFormat::PIXEL_FORMAT_NV21:
          // case CameraFormat::PIXEL_FORMAT_MT21:
          // case CameraFormat::PIXEL_FORMAT_YUV420P9:
          // case CameraFormat::PIXEL_FORMAT_YUV420P10:
          // case CameraFormat::PIXEL_FORMAT_YUV420P12:
          // case CameraFormat::PIXEL_FORMAT_P016LE:
          return Sizei(2, 2);

        case CameraFormat::PIXEL_FORMAT_UNKNOWN:
        case CameraFormat::PIXEL_FORMAT_UYVY:
        case CameraFormat::PIXEL_FORMAT_YUY2:
        case CameraFormat::PIXEL_FORMAT_ARGB:
        // case CameraFormat::PIXEL_FORMAT_XRGB:
        case CameraFormat::PIXEL_FORMAT_RGB24:
        case CameraFormat::PIXEL_FORMAT_RGB32:
        case CameraFormat::PIXEL_FORMAT_MJPEG:
          // case CameraFormat::PIXEL_FORMAT_ABGR:
          // case CameraFormat::PIXEL_FORMAT_XBGR:
          break;
      }
  }
  NOTREACHED();
  return Sizei();
}

int BytesPerElement(CameraFormat camera_format, size_t plane) {
  DCHECK(IsValidPlane(plane, camera_format));

  CameraFormat::PixelFormat pixel_format = camera_format.pixel_format();
  switch (pixel_format) {
    case CameraFormat::PIXEL_FORMAT_ARGB:
    // case CameraFormat::PIXEL_FORMAT_XRGB:
    case CameraFormat::PIXEL_FORMAT_RGB32:
      // case CameraFormat::PIXEL_FORMAT_ABGR:
      // case CameraFormat::PIXEL_FORMAT_XBGR:
      return 4;
    case CameraFormat::PIXEL_FORMAT_RGB24:
      return 3;
    // case CameraFormat::PIXEL_FORMAT_Y16:
    case CameraFormat::PIXEL_FORMAT_UYVY:
    case CameraFormat::PIXEL_FORMAT_YUY2:
      // case CameraFormat::PIXEL_FORMAT_YUV420P9:
      // case CameraFormat::PIXEL_FORMAT_YUV422P9:
      // case CameraFormat::PIXEL_FORMAT_YUV444P9:
      // case CameraFormat::PIXEL_FORMAT_YUV420P10:
      // case CameraFormat::PIXEL_FORMAT_YUV422P10:
      // case CameraFormat::PIXEL_FORMAT_YUV444P10:
      // case CameraFormat::PIXEL_FORMAT_YUV420P12:
      // case CameraFormat::PIXEL_FORMAT_YUV422P12:
      // case CameraFormat::PIXEL_FORMAT_YUV444P12:
      // case CameraFormat::PIXEL_FORMAT_P016LE:
      return 2;
    case CameraFormat::PIXEL_FORMAT_NV12:
    case CameraFormat::PIXEL_FORMAT_NV21: {
      // case CameraFormat::PIXEL_FORMAT_MT21: {
      static const int bytes_per_element[] = {1, 2};
      DCHECK_LT(plane, base::size(bytes_per_element));
      return bytes_per_element[plane];
    }
    case CameraFormat::PIXEL_FORMAT_YV12:
    case CameraFormat::PIXEL_FORMAT_I420:
      // case CameraFormat::PIXEL_FORMAT_I422:
      // case CameraFormat::PIXEL_FORMAT_I420A:
      // case CameraFormat::PIXEL_FORMAT_I444:
      return 1;
    case CameraFormat::PIXEL_FORMAT_MJPEG:
      return 0;
    case CameraFormat::PIXEL_FORMAT_UNKNOWN:
      break;
  }
  NOTREACHED();
  return 0;
}

Sizei PlaneSize(CameraFormat camera_format, size_t plane) {
  DCHECK(IsValidPlane(plane, camera_format));

  int width = camera_format.width();
  int height = camera_format.height();
  if (RequiresEvenSizeAllocation(camera_format)) {
    // Align to multiple-of-two size overall. This ensures that non-subsampled
    // planes can be addressed by pixel with the same scaling as the subsampled
    // planes.
    width = ::base::bits::Align(width, 2);
    height = ::base::bits::Align(height, 2);
  }

  const Sizei subsample = SampleSize(camera_format, plane);
  DCHECK(width % subsample.width() == 0);
  DCHECK(height % subsample.height() == 0);
  return Sizei(
      BytesPerElement(camera_format, plane) * width / subsample.width(),
      height / subsample.height());
}

}  // namespace

size_t AllocationSize(CameraFormat camera_format) {
  size_t total = 0;
  for (size_t i = 0; i < NumPlanes(camera_format); ++i)
    total += PlaneSize(camera_format, i).area();
  return total;
}

}  // namespace felicia