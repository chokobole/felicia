#ifndef FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_H_
#define FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_H_

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"

#include "felicia/python/type_conversion/callback_forward.h"

namespace py = pybind11;

namespace felicia {

template <typename R, typename... Args>
class PyCallback<R(Args...)> {
 public:
  explicit PyCallback(const py::function& func)
      : func_(py::cast<std::function<R(Args...)>>(func)) {}

  R Invoke(Args... args) { return func_(std::forward<Args>(args)...); }

 private:
  std::function<R(Args...)> func_;
};

}  // namespace felicia

#endif  // FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_H_