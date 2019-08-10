#ifndef FELICIA_PYTHON_DRIVERS_PY_H_
#define FELICIA_PYTHON_DRIVERS_PY_H_

#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace felicia {
namespace drivers {

void AddDrivers(py::module& m);

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_PYTHON_DRIVERS_PY_H_