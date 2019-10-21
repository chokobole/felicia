#ifndef FELICIA_PYTHON_COMMAND_LINE_INTERFACE_COMMAND_LINE_INTERFACE_PY_H_
#define FELICIA_PYTHON_COMMAND_LINE_INTERFACE_COMMAND_LINE_INTERFACE_PY_H_

#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace felicia {

void AddCommandLineInterface(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_COMMAND_LINE_INTERFACE_COMMAND_LINE_INTERFACE_PY_H_