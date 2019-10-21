#include "felicia/python/communication/message_util.h"

namespace felicia {

std::string GetMessageTypeNameFromPyObject(const py::object& message_type,
                                           TopicInfo::ImplType impl_type) {
  std::string message_type_name;
  switch (impl_type) {
    case TopicInfo::PROTOBUF: {
      message_type_name =
          py::str(message_type.attr("DESCRIPTOR").attr("full_name"));
      break;
    }
    case TopicInfo::ROS: {
      message_type_name = py::str(message_type.attr("_type"));
      break;
    }
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MIN_SENTINEL_DO_NOT_USE_:
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MAX_SENTINEL_DO_NOT_USE_:
      break;
  }
  return message_type_name;
}

std::string GetMessageMD5SumFromPyObject(const py::object& message_type,
                                         TopicInfo::ImplType impl_type) {
  std::string md5sum;
  switch (impl_type) {
    case TopicInfo::PROTOBUF:
      break;
    case TopicInfo::ROS: {
      md5sum = py::str(message_type.attr("_md5sum"));
      break;
    }
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MIN_SENTINEL_DO_NOT_USE_:
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MAX_SENTINEL_DO_NOT_USE_:
      break;
  }
  return md5sum;
}

std::string GetMessageDefinitionFromPyObject(const py::object& message_type,
                                             TopicInfo::ImplType impl_type) {
  std::string definition;
  switch (impl_type) {
    case TopicInfo::PROTOBUF:
      break;
    case TopicInfo::ROS: {
      definition = py::str(message_type.attr("_full_text"));
      break;
    }
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MIN_SENTINEL_DO_NOT_USE_:
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MAX_SENTINEL_DO_NOT_USE_:
      break;
  }
  return definition;
}

}  // namespace felicia