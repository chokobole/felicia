// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(FEL_PY_BINDING)

#include "pybind11/pybind11.h"

#include "felicia/python/type_conversion/protobuf.h"
#include "felicia/python/type_conversion/test/test.pb.h"

namespace py = pybind11;

SUPPORT_PROTOBUF_TYPE_CAST(felicia::Test, Test,
                           felicia.python.type_conversion.test.test_pb2)

namespace pybind11 {
namespace detail {
template <>
struct type_caster<felicia::Test2> {
 public:
  PYBIND11_TYPE_CASTER(felicia::Test2, _("felicia::Test2"));

  bool load(handle src, bool convert) {
    auto obj = py::reinterpret_borrow<py::object>(src);
    value.set_id(py::cast<py::int_>(obj.attr("id")));
    value.set_content(py::cast<py::str>(obj.attr("content")));

    return !PyErr_Occurred();
  }

  static handle cast(const felicia::Test2& src,
                     return_value_policy /* policy */, handle /* parent */) {
    pybind11::object object =
        pybind11::module::import("felicia.python.type_conversion.test.test_pb2")
            .attr("Test2")();
    object.attr("id") = src.id();
    object.attr("content") = src.content();
    object.inc_ref();

    return std::move(object);
  }
};

}  // namespace detail
}  // namespace pybind11

namespace felicia {

Test test(const Test& test) {
  // std::cout << test.DebugString() << std::endl;
  Test new_test;
  new_test.set_id(test.id() + 1);
  new_test.set_content(test.content());
  return new_test;
}

Test2 test2(const Test2& test2) {
  // std::cout << test2.DebugString() << std::endl;
  Test2 new_test2;
  new_test2.set_id(test2.id() + 1);
  new_test2.set_content(test2.content());
  return new_test2;
}

PYBIND11_MODULE(protobuf_py, m) { m.def("test", &test).def("test2", &test2); }

}  // namespace felicia

#endif  // defined(FEL_PY_BINDING)