#include "felicia/python/channel_py.h"

#include "felicia/core/channel/settings.h"

namespace felicia {

void AddChannel(py::module& m) {
  py::module channel = m.def_submodule("channel");

  py::class_<channel::WSSettings>(channel, "WSSettings")
      .def(py::init<>())
      .def_readwrite("permessage_deflate_enabled",
                     &channel::WSSettings::permessage_deflate_enabled)
      .def_readwrite("server_max_window_bits",
                     &channel::WSSettings::server_max_window_bits);

  py::class_<channel::Settings>(channel, "Settings")
      .def(py::init<>())
      .def_readwrite("ws_settings", &channel::Settings::ws_settings);
}

}  // namespace felicia