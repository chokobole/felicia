#include "felicia/python/drivers_py.h"

#include "felicia/python/drivers/camera_py.h"

namespace felicia {
namespace drivers {

void AddDrivers(py::module& m) {
  py::module drivers = m.def_submodule("drivers");

  AddCamera(drivers);
}

}  // namespace drivers
}  // namespace felicia