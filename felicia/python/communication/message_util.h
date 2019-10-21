#ifndef FELICIA_PYTHON_COMMUNICATION_MESSAGE_UTIL_H_
#define FELICIA_PYTHON_COMMUNICATION_MESSAGE_UTIL_H_

#include "pybind11/pybind11.h"

#include "felicia/core/protobuf/master_data.pb.h"

namespace py = pybind11;

namespace felicia {

std::string GetMessageTypeNameFromPyObject(const py::object& message_type,
                                           TopicInfo::ImplType impl_type);

std::string GetMessageMD5SumFromPyObject(const py::object& message_type,
                                         TopicInfo::ImplType impl_type);

std::string GetMessageDefinitionFromPyObject(const py::object& message_type,
                                             TopicInfo::ImplType impl_type);

}  // namespace felicia

#endif  // FELICIA_PYTHON_COMMUNICATION_MESSAGE_UTIL_H_