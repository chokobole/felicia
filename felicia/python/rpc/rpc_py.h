#ifndef FELICIA_PYTHON_RPC_RPC_PY_H_
#define FELICIA_PYTHON_RPC_RPC_PY_H_

#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace felicia {

void AddRpc(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_RPC_RPC_PY_H_