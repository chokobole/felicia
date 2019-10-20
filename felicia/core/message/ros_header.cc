#if defined(HAS_ROS)

#include "felicia/core/message/ros_header.h"

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

#define EXPECT_NOT_EMPTY(property) \
  if (property.empty()) return errors::InvalidArgument(#property " is empty")

#define EXPECT_BOOL(property)                        \
  if (property != "1" && property != "0")            \
  return errors::InvalidArgument(base::StringPrintf( \
      #property " has a value(%s) other than 1 or 0", property.c_str()))

#define EXPECT_MATCH(property)                                    \
  if (property != expected.property)                              \
  return errors::InvalidArgument(                                 \
      base::StringPrintf(#property " is not matched : %s vs %s.", \
                         property.c_str(), expected.property.c_str()))

namespace felicia {

RosHeader::~RosHeader() = default;

void RosHeader::WriteToBuffer(std::string* buffer) const {
  ros::M_string header_map;
  WriteToHeaderMap(&header_map);
  boost::shared_array<uint8_t> tmp_buffer;
  uint32_t buffer_len;
  ros::Header::write(header_map, tmp_buffer, buffer_len);
  buffer->resize(buffer_len);
  memcpy(const_cast<char*>(buffer->c_str()), tmp_buffer.get(), buffer_len);
}

Status RosHeader::ReadFromBuffer(const char* buf, size_t buf_len) {
  ros::Header ros_header;
  std::string error_message;
  if (!ros_header.parse(reinterpret_cast<uint8_t*>(const_cast<char*>(buf)),
                        buf_len, error_message)) {
    return errors::Aborted(base::StringPrintf("Failed to parse ROS Header: %s.",
                                              error_message.c_str()));
  }

  ReadFromRosHeader(ros_header);
  return Status::OK();
}

void RosHeader::WriteToHeaderMap(ros::M_string* header_map) const {
  (*header_map)["callerid"] = callerid;
  (*header_map)["md5sum"] = md5sum;
}

void RosHeader::ReadFromRosHeader(const ros::Header& ros_header) {
  ros_header.getValue("md5sum", md5sum);
  ros_header.getValue("callerid", callerid);
}

Status RosHeader::Validate(const RosHeader& expected) const {
  EXPECT_NOT_EMPTY(callerid);
  EXPECT_NOT_EMPTY(md5sum);

  if (md5sum != expected.md5sum && md5sum != "*" && expected.md5sum != "*") {
    return errors::InvalidArgument(
        base::StringPrintf("md5sum is not matched :%s vs %s.", md5sum.c_str(),
                           expected.md5sum.c_str()));
  }

  return Status::OK();
}

void RosTopicHeader::WriteToHeaderMap(ros::M_string* header_map) const {
  RosHeader::WriteToHeaderMap(header_map);
  (*header_map)["topic"] = topic;
  (*header_map)["type"] = type;
}

void RosTopicHeader::ReadFromRosHeader(const ros::Header& ros_header) {
  RosHeader::ReadFromRosHeader(ros_header);
  ros_header.getValue("topic", topic);
  ros_header.getValue("type", type);
}

Status RosTopicHeader::Validate(const RosTopicHeader& expected) const {
  EXPECT_NOT_EMPTY(topic);
  EXPECT_NOT_EMPTY(type);
  EXPECT_MATCH(topic);
  EXPECT_MATCH(type);

  return RosHeader::Validate(expected);
}

void RosTopicRequestHeader::WriteToHeaderMap(ros::M_string* header_map) const {
  RosTopicHeader::WriteToHeaderMap(header_map);
  (*header_map)["tcp_nodelay"] = tcp_nodelay;
}

void RosTopicRequestHeader::ReadFromRosHeader(const ros::Header& ros_header) {
  RosTopicHeader::ReadFromRosHeader(ros_header);
  ros_header.getValue("tcp_nodelay", tcp_nodelay);
}

Status RosTopicRequestHeader::Validate(const RosTopicHeader& expected) const {
  EXPECT_NOT_EMPTY(tcp_nodelay);
  EXPECT_BOOL(tcp_nodelay);

  return RosTopicHeader::Validate(expected);
}

void RosTopicResponseHeader::WriteToHeaderMap(ros::M_string* header_map) const {
  RosTopicHeader::WriteToHeaderMap(header_map);
  if (error.empty()) {
    (*header_map)["message_definition"] = message_definition;
    (*header_map)["latching"] = latching;
  } else {
    (*header_map)["error"] = error;
  }
}

Status RosTopicResponseHeader::Validate(const RosTopicHeader& expected) const {
  EXPECT_NOT_EMPTY(latching);
  EXPECT_NOT_EMPTY(message_definition);
  EXPECT_BOOL(latching);

  return RosTopicHeader::Validate(expected);
}

void RosTopicResponseHeader::ReadFromRosHeader(const ros::Header& ros_header) {
  RosTopicHeader::ReadFromRosHeader(ros_header);
  ros_header.getValue("error", error);
  ros_header.getValue("message_definition", message_definition);
  ros_header.getValue("latching", latching);
}

void RosServiceRequestHeader::WriteToHeaderMap(
    ros::M_string* header_map) const {
  RosHeader::WriteToHeaderMap(header_map);
  (*header_map)["persistent"] = persistent;
  (*header_map)["service"] = service;
}

void RosServiceRequestHeader::ReadFromRosHeader(const ros::Header& ros_header) {
  RosHeader::ReadFromRosHeader(ros_header);
  ros_header.getValue("persistent", persistent);
  ros_header.getValue("service", service);
}

Status RosServiceRequestHeader::Validate(
    const RosServiceRequestHeader& expected) const {
  EXPECT_NOT_EMPTY(persistent);
  EXPECT_NOT_EMPTY(service);
  EXPECT_BOOL(persistent);

  return RosHeader::Validate(expected);
}

void RosServiceResponseHeader::WriteToHeaderMap(
    ros::M_string* header_map) const {
  RosHeader::WriteToHeaderMap(header_map);
  if (error.empty()) {
    (*header_map)["request_type"] = request_type;
    (*header_map)["response_type"] = response_type;
    (*header_map)["type"] = type;
  } else {
    (*header_map)["error"] = error;
  }
}

void RosServiceResponseHeader::ReadFromRosHeader(
    const ros::Header& ros_header) {
  RosHeader::ReadFromRosHeader(ros_header);
  ros_header.getValue("error", error);
  ros_header.getValue("request_type", request_type);
  ros_header.getValue("response_type", response_type);
  ros_header.getValue("type", type);
}

Status RosServiceResponseHeader::Validate(
    const RosServiceResponseHeader& expected) const {
  EXPECT_NOT_EMPTY(request_type);
  EXPECT_NOT_EMPTY(response_type);
  EXPECT_NOT_EMPTY(type);

  return RosHeader::Validate(expected);
}

}  // namespace felicia

#undef EXPECT_NOT_EMPTY
#undef EXPECT_BOOL
#undef EXPECT_MATCH

#endif  // defined(HAS_ROS)