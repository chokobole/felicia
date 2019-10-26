#ifndef FELICIA_PYTHON_TYPE_CONVERSION_UTIL_H_
#define FELICIA_PYTHON_TYPE_CONVERSION_UTIL_H_

#include "pybind11/pybind11.h"

#include "third_party/chromium/base/logging.h"

namespace py = pybind11;

namespace felicia {

bool IsGilHeld();

}  // namespace felicia

#define FEL_CALL_PURE_FUNCTION(cname, name)                               \
  pybind11::pybind11_fail("Tried to call pure virtual function \"" #cname \
                          "::" name "\"")

// This is similar with PYBIND11_OVERLOAD_INT macro. but it doesn't acquire gil.
#define FEL_OVERLOAD_INT_WITHOUT_GIL(ret_type, cname, name, ...)           \
  do {                                                                     \
    DCHECK(IsGilHeld());                                                   \
    pybind11::function overload =                                          \
        pybind11::get_overload(static_cast<const cname *>(this), name);    \
    if (overload) {                                                        \
      auto o = overload(__VA_ARGS__);                                      \
      if (pybind11::detail::cast_is_temporary_value_reference<             \
              ret_type>::value) {                                          \
        static pybind11::detail::overload_caster_t<ret_type> caster;       \
        return pybind11::detail::cast_ref<ret_type>(std::move(o), caster); \
      } else                                                               \
        return pybind11::detail::cast_safe<ret_type>(std::move(o));        \
    }                                                                      \
  } while (0)

#define FEL_OVERLOAD_INT_WITH_GIL(...) \
  do {                                 \
    PYBIND11_OVERLOAD_INT(__VA_ARGS__) \
  } while (0)

#endif  // FELICIA_PYTHON_TYPE_CONVERSION_UTIL_H_