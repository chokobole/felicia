#ifndef FELICIA_PYTHON_TYPE_CONVERSION_PROTOBUF_H_
#define FELICIA_PYTHON_TYPE_CONVERSION_PROTOBUF_H_

#include "pybind11/pybind11.h"

#define SUPPORT_PROTOBUF_TYPE_CAST(CcType, PyType, Module)                 \
  namespace pybind11 {                                                     \
  namespace detail {                                                       \
  template <>                                                              \
  struct type_caster<CcType> {                                             \
   public:                                                                 \
    PYBIND11_TYPE_CASTER(CcType, _(#CcType));                              \
                                                                           \
    bool load(handle src, bool convert) {                                  \
      std::string text = pybind11::str(src.attr("SerializeToString")());   \
      if (PyErr_Occurred()) {                                              \
        return false;                                                      \
      }                                                                    \
                                                                           \
      bool ret = value.ParseFromString(text);                              \
      return ret;                                                          \
    }                                                                      \
                                                                           \
    static handle cast(CcType src, return_value_policy /* policy */,       \
                       handle /* parent */) {                              \
      std::string text;                                                    \
      bool ret = src.SerializeToString(&text);                             \
      if (!ret) {                                                          \
        PyErr_SetString(PyExc_ValueError,                                  \
                        "Failed to serialized to string from c++ " #CcType \
                        " to python " #PyType ".");                        \
        return nullptr;                                                    \
      }                                                                    \
                                                                           \
      pybind11::object object =                                            \
          pybind11::module::import(#Module).attr(#PyType)();               \
      object.attr("ParseFromString")(pybind11::bytes(text));               \
      object.inc_ref();                                                    \
      return object;                                                       \
    }                                                                      \
  };                                                                       \
  }                                                                        \
  }

#endif  // FELICIA_PYTHON_TYPE_CONVERSION_PROTOBUF_H_