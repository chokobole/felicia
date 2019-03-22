#ifndef FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_H_
#define FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_H_

#include "pybind11/pybind11.h"

#include <type_traits>

#include "felicia/python/type_conversion/callback_forward.h"

namespace py = pybind11;

namespace felicia {

template <typename R, typename... Args>
class PYBIND11_EXPORT PyCallback<R(Args...)> {
 public:
  explicit PyCallback(py::function func) : func_(func) {}

  R Invoke(Args... args) { return func_(std::forward<Args>(args)...); }

 private:
  py::function func_;
};

template <typename... Args>
class PYBIND11_EXPORT PyCallback<void(Args...)> {
 public:
  explicit PyCallback(py::function func) : func_(func) {}

  void Invoke(Args... args) { func_(std::forward<Args>(args)...); }

 private:
  py::function func_;
};

}  // namespace felicia

#endif  // FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_H_