// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_PYTHON_COMMUNICATION_MESSAGE_UTIL_H_
#define FELICIA_PYTHON_COMMUNICATION_MESSAGE_UTIL_H_

#if defined(FEL_PY_BINDING)

#include "pybind11/pybind11.h"

#include "third_party/chromium/base/compiler_specific.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace py = pybind11;

namespace felicia {

Status Serialize(const py::object& message, TopicInfo::ImplType impl_type,
                 std::string* text) WARN_UNUSED_RESULT;

Status Deserialize(const std::string& text, TopicInfo::ImplType impl_type,
                   py::object* message) WARN_UNUSED_RESULT;

std::string GetMessageTypeNameFromPyObject(const py::object& message_type,
                                           TopicInfo::ImplType impl_type);

std::string GetMessageMD5SumFromPyObject(const py::object& message_type,
                                         TopicInfo::ImplType impl_type);

std::string GetMessageDefinitionFromPyObject(const py::object& message_type,
                                             TopicInfo::ImplType impl_type);

}  // namespace felicia

#endif  // defined(FEL_PY_BINDING)

#endif  // FELICIA_PYTHON_COMMUNICATION_MESSAGE_UTIL_H_