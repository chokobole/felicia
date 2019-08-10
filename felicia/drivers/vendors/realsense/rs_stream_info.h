#ifndef FELICIA_DRIVERS_VENDORS_REALSENSE_RS_STREAM_INFO_H_
#define FELICIA_DRIVERS_VENDORS_REALSENSE_RS_STREAM_INFO_H_

#include <librealsense2/rs.hpp>

namespace felicia {
namespace drivers {

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

  bool operator==(const RsStreamInfo& other) const {
    return stream_type == other.stream_type &&
           stream_index == other.stream_index;
  }
  rs2_stream stream_type;
  int stream_index;
};

const RsStreamInfo RS_COLOR{RS2_STREAM_COLOR, 0};
const RsStreamInfo RS_DEPTH{RS2_STREAM_DEPTH, 0};
const RsStreamInfo RS_INFRA1{RS2_STREAM_INFRARED, 1};
const RsStreamInfo RS_INFRA2{RS2_STREAM_INFRARED, 2};
const RsStreamInfo RS_FISHEYE{RS2_STREAM_FISHEYE, 0};
const RsStreamInfo RS_FISHEYE1{RS2_STREAM_FISHEYE, 1};
const RsStreamInfo RS_FISHEYE2{RS2_STREAM_FISHEYE, 2};
const RsStreamInfo RS_GYRO{RS2_STREAM_GYRO, 0};
const RsStreamInfo RS_ACCEL{RS2_STREAM_ACCEL, 0};
const RsStreamInfo RS_POSE{RS2_STREAM_POSE, 0};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_REALSENSE_RS_STREAM_INFO_H_