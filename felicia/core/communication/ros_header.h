#if defined(HAS_ROS)

#ifndef FELICIA_CORE_COMMUNICATION_ROS_HEADER_H_
#define FELICIA_CORE_COMMUNICATION_ROS_HEADER_H_

#include <string>

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

struct ROSHeader {
  std::string topic;
  std::string md5sum;
  std::string message_definition;
  std::string latching;
  std::string callerid;
  std::string type;
  std::string tcp_nodelay;
  std::string error;
};

EXPORT Status WriteROSHeaderToBuffer(const ROSHeader& ros_header,
                                     std::string* buffer, bool to_publisher);

EXPORT Status ReadROSHeaderFromBuffer(const std::string& buffer,
                                      ROSHeader* header, bool from_subscriber);

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_ROS_HEADER_H_

#endif  // defined(HAS_ROS)