#if defined(HAS_ROS)

#if defined(HAS_ROS)
#include <ros/header.h>
#endif  // defined(HAS_ROS)

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/communication/ros_header.h"
#include "felicia/core/lib/error/errors.h"

namespace felicia {

Status WriteROSHeaderToBuffer(const ROSHeader& ros_header, std::string* buffer,
                              bool to_publisher) {
  ros::M_string header;
  if (ros_header.error.empty()) {
    header["topic"] = ros_header.topic;
    header["md5sum"] = ros_header.md5sum;
    header["callerid"] = ros_header.callerid;
    header["type"] = ros_header.type;
    if (to_publisher) {
      header["tcp_nodelay"] = ros_header.tcp_nodelay;
    } else {
      header["message_definition"] = ros_header.message_definition;
      header["latching"] = ros_header.latching;
    }
  } else {
    header["error"] = ros_header.error;
  }

  boost::shared_array<uint8_t> tmp_buffer;
  uint32_t buffer_len;
  ros::Header::write(header, tmp_buffer, buffer_len);
  buffer->resize(buffer_len);
  memcpy(const_cast<char*>(buffer->c_str()), tmp_buffer.get(), buffer_len);
  return Status::OK();
}

Status ReadROSHeaderFromBuffer(const std::string& buffer, ROSHeader* header,
                               bool from_subscriber) {
  ros::Header tmp_header;
  std::string error_message;
  if (!tmp_header.parse(
          reinterpret_cast<uint8_t*>(const_cast<char*>(buffer.c_str())),
          buffer.length(), error_message)) {
    return errors::Unavailable(base::StringPrintf(
        "Failed to parse ROS Header: %s.", error_message.c_str()));
  } else {
    if (tmp_header.getValue("error", error_message)) {
      return errors::Unavailable(base::StringPrintf(
          "ROS Header contains error: %s.", error_message.c_str()));
    }
  }

  tmp_header.getValue("topic", header->topic);
  tmp_header.getValue("md5sum", header->md5sum);
  tmp_header.getValue("callerid", header->callerid);
  tmp_header.getValue("type", header->type);
  if (from_subscriber) {
    tmp_header.getValue("tcp_nodelay", header->tcp_nodelay);
  } else {
    tmp_header.getValue("message_definition", header->message_definition);
    tmp_header.getValue("latching", header->latching);
  }

  return Status::OK();
}

}  // namespace felicia

#endif  // defined(HAS_ROS)