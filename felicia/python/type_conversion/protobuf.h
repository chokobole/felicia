#ifndef FELICIA_PYTHON_TYPE_CONVERSION_PROTOBUF_H_
#define FELICIA_PYTHON_TYPE_CONVERSION_PROTOBUF_H_

#include "pybind11/pybind11.h"

#include "felicia/core/protobuf/master_data.pb.h"
#include "felicia/core/protobuf/ui.pb.h"

#define SUPPORT_PROTOBUF_ENUM_TYPE_CAST(CcType, PyType)              \
  namespace pybind11 {                                               \
  namespace detail {                                                 \
  template <>                                                        \
  struct type_caster<CcType> {                                       \
   public:                                                           \
    PYBIND11_TYPE_CASTER(CcType, _(#CcType));                        \
                                                                     \
    bool load(handle src, bool convert) {                            \
      PyObject *source = src.ptr();                                  \
      value = static_cast<CcType>(PyLong_AsLong(source));            \
      return true;                                                   \
    }                                                                \
                                                                     \
    static handle cast(CcType src, return_value_policy /* policy */, \
                       handle /* parent */) {                        \
      return PyLong_FromLong(src);                                   \
    }                                                                \
  };                                                                 \
  }                                                                  \
  }

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
      return value.ParseFromString(text);                                  \
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
      return std::move(object);                                            \
    }                                                                      \
  };                                                                       \
  }                                                                        \
  }

SUPPORT_PROTOBUF_TYPE_CAST(felicia::ChannelDef, ChannelDef,
                           felicia.core.protobuf.channel_pb2)

SUPPORT_PROTOBUF_TYPE_CAST(felicia::IPEndPoint, IPEndPoint,
                           felicia.core.protobuf.channel_pb2)

SUPPORT_PROTOBUF_TYPE_CAST(felicia::NodeInfo, NodeInfo,
                           felicia.core.protobuf.master_data_pb2)

SUPPORT_PROTOBUF_TYPE_CAST(felicia::ServiceInfo, ServiceInfo,
                           felicia.core.protobuf.master_data_pb2)

SUPPORT_PROTOBUF_ENUM_TYPE_CAST(felicia::ChannelDef::Type, ChannelDef.Type)

SUPPORT_PROTOBUF_ENUM_TYPE_CAST(felicia::PixelFormat, PixelFormat)

SUPPORT_PROTOBUF_ENUM_TYPE_CAST(felicia::ServiceInfo::Status,
                                ServiceInfo.Status)

SUPPORT_PROTOBUF_ENUM_TYPE_CAST(felicia::TopicInfo::ImplType,
                                TopicInfo.ImplType)

#endif  // FELICIA_PYTHON_TYPE_CONVERSION_PROTOBUF_H_