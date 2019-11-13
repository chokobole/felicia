// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_PYTHON_COMMUNICATION_SERIALIZED_MESSAGE_PUBLISHER_PY_H_
#define FELICIA_PYTHON_COMMUNICATION_SERIALIZED_MESSAGE_PUBLISHER_PY_H_

#include "pybind11/pybind11.h"

#include "felicia/core/communication/serialized_message_publisher.h"

namespace py = pybind11;

namespace felicia {

class PySerializedMessagePublisher : public SerializedMessagePublisher {
 public:
  explicit PySerializedMessagePublisher(
      py::object message_type,
      TopicInfo::ImplType impl_type = TopicInfo::PROTOBUF);

  void RequestPublish(const NodeInfo& node_info, const std::string& topic,
                      int channel_types,
                      const communication::Settings& settings,
                      py::function py_callback = py::none());

  void RequestUnpublish(const NodeInfo& node_info, const std::string& topic,
                        py::function py_callback = py::none());

  void PublishFromSerialized(py::object message,
                             py::function py_callback = py::none());
};

void AddSerializedMessagePublisher(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_COMMUNICATION_SERIALIZED_MESSAGE_PUBLISHER_PY_H_