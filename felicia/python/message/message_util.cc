#include "felicia/python/message/message_util.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

Status Serialize(const py::object& message, TopicInfo::ImplType impl_type,
                 std::string* text) {
  try {
    py::handle hanle;
    py::object object;
    switch (impl_type) {
      case TopicInfo::PROTOBUF: {
        *text = py::str(message.attr("SerializeToString")());
        break;
      }
      case TopicInfo::ROS: {
        py::object buffer = py::module::import("io").attr("BytesIO")();
        message.attr("serialize")(buffer);
        *text = py::str(buffer.attr("getvalue")());
        break;
      }
      case TopicInfo_ImplType_TopicInfo_ImplType_INT_MIN_SENTINEL_DO_NOT_USE_:
      case TopicInfo_ImplType_TopicInfo_ImplType_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }
  } catch (py::error_already_set& e) {
    return errors::Unknown(e.what());
  }
  return Status::OK();
}

Status Deserialize(const std::string& text, TopicInfo::ImplType impl_type,
                   py::object* message) {
  try {
    switch (impl_type) {
      case TopicInfo::PROTOBUF: {
        message->attr("ParseFromString")(py::bytes(text));
        break;
      }
      case TopicInfo::ROS: {
        message->attr("deserialize")(py::bytes(text));
        break;
      }
      case TopicInfo_ImplType_TopicInfo_ImplType_INT_MIN_SENTINEL_DO_NOT_USE_:
      case TopicInfo_ImplType_TopicInfo_ImplType_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }
  } catch (py::error_already_set& e) {
    return errors::Unknown(e.what());
  }
  return Status::OK();
}

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