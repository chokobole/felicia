#include "felicia/python/drivers/drivers_py.h"

#include "felicia/python/drivers/camera_py.h"

namespace felicia {

void AddDrivers(py::module& m) {
  py::module drivers = m.def_submodule("drivers");

  drivers::AddCamera(drivers);
}

}  // namespace felicia