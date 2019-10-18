#if defined(HAS_ROS)

#ifndef FELICIA_CORE_CHANNEL_ROS_HEADER_H_
#define FELICIA_CORE_CHANNEL_ROS_HEADER_H_

#include <ros/header.h>

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {
struct EXPORT RosHeader {
  std::string callerid;
  std::string md5sum;

  virtual ~RosHeader();

  void WriteToBuffer(std::string* buffer) const;
  Status ReadFromBuffer(const std::string& buffer);

  virtual void WriteToHeaderMap(ros::M_string* header_map) const;
  virtual void ReadFromRosHeader(const ros::Header& ros_header);
  Status Validate(const RosHeader& expected) const;
};

struct EXPORT RosTopicHeader : public RosHeader {
  std::string topic;
  std::string type;

  void WriteToHeaderMap(ros::M_string* header_map) const override;
  void ReadFromRosHeader(const ros::Header& ros_header) override;
  Status Validate(const RosTopicHeader& expected) const;
};

struct EXPORT RosTopicRequestHeader : public RosTopicHeader {
  std::string tcp_nodelay;

  void WriteToHeaderMap(ros::M_string* header_map) const override;
  void ReadFromRosHeader(const ros::Header& ros_header) override;
  Status Validate(const RosTopicHeader& expected) const;
};

struct EXPORT RosTopicResponseHeader : public RosTopicHeader {
  std::string error;
  std::string latching;
  std::string message_definition;

  void SetValuesFrom(const RosTopicRequestHeader& request_header);

  void WriteToHeaderMap(ros::M_string* header_map) const override;
  void ReadFromRosHeader(const ros::Header& ros_header) override;
  Status Validate(const RosTopicHeader& expected) const;
};

struct EXPORT RosServiceRequestHeader : public RosHeader {
  std::string persistent;
  std::string service;

  void WriteToHeaderMap(ros::M_string* header_map) const override;
  void ReadFromRosHeader(const ros::Header& ros_header) override;
  Status Validate(const RosServiceRequestHeader& expected) const;
};

struct EXPORT RosServiceResponseHeader : public RosHeader {
  std::string error;
  std::string request_type;
  std::string response_type;
  std::string type;

  void SetValuesFrom(const RosServiceRequestHeader& request_header);

  void WriteToHeaderMap(ros::M_string* header_map) const override;
  void ReadFromRosHeader(const ros::Header& ros_header) override;
  Status Validate(const RosServiceResponseHeader& expected) const;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_ROS_HEADER_H_

#endif  // defined(HAS_ROS)