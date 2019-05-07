#ifndef FELICIA_DRIVERS_CAMERA_REALSENSE_RS_STREAM_INFO_H_
#define FELICIA_DRIVERS_CAMERA_REALSENSE_RS_STREAM_INFO_H_

#include <librealsense2/rs.hpp>

#include "third_party/chromium/base/containers/flat_map.h"

#include "felicia/drivers/camera/camera_format.h"

namespace felicia {

struct RsStreamInfo {
  RsStreamInfo() = default;
  RsStreamInfo(rs2_stream stream_type, int stream_index)
      : stream_type(stream_type), stream_index(stream_index) {}

  bool operator<(const RsStreamInfo& other) const {
    if (stream_type < other.stream_type)
      return true;
    else if (stream_type > other.stream_type)
      return false;
    return stream_index < other.stream_index;
  }
  rs2_stream stream_type;
  int stream_index;
};

const RsStreamInfo COLOR{RS2_STREAM_COLOR, 0};
const RsStreamInfo DEPTH{RS2_STREAM_DEPTH, 0};
const RsStreamInfo INFRA1{RS2_STREAM_INFRARED, 1};
const RsStreamInfo INFRA2{RS2_STREAM_INFRARED, 2};
const RsStreamInfo FISHEYE{RS2_STREAM_FISHEYE, 0};
const RsStreamInfo FISHEYE1{RS2_STREAM_FISHEYE, 1};
const RsStreamInfo FISHEYE2{RS2_STREAM_FISHEYE, 2};
const RsStreamInfo GYRO{RS2_STREAM_GYRO, 0};
const RsStreamInfo ACCEL{RS2_STREAM_ACCEL, 0};
const RsStreamInfo POSE{RS2_STREAM_POSE, 0};

using RsCameraFormatMap =
    ::base::flat_map<RsStreamInfo, std::vector<CameraFormat>>;

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_REALSENSE_RS_STREAM_INFO_H_