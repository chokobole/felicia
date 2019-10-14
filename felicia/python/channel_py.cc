#include "felicia/python/channel_py.h"

#include "felicia/core/channel/settings.h"

namespace felicia {

void AddChannel(py::module& m) {
  py::module channel = m.def_submodule("channel");

  py::class_<channel::TCPSettings>(channel, "TCPSettings")
#if defined(FEL_NO_SSL)
      .def(py::init<>());
#else
      .def(py::init<>())
      .def_readwrite("use_ssl", &channel::TCPSettings::use_ssl);
#endif

#if defined(OS_POSIX)
  py::class_<channel::UDSSettings>(channel, "UDSSettings").def(py::init<>());
#endif

  py::class_<channel::WSSettings>(channel, "WSSettings")
      .def(py::init<>())
      .def_readwrite("permessage_deflate_enabled",
                     &channel::WSSettings::permessage_deflate_enabled)
      .def_readwrite("server_max_window_bits",
                     &channel::WSSettings::server_max_window_bits);

  py::class_<channel::ShmSettings>(channel, "ShmSettings")
      .def(py::init<>())
      .def_readwrite("shm_size", &channel::ShmSettings::shm_size);

  py::class_<channel::Settings>(channel, "Settings")
      .def(py::init<>())
      .def_readwrite("tcp_settings", &channel::Settings::tcp_settings)
      .def_readwrite("ws_settings", &channel::Settings::ws_settings)
#if defined(OS_POSIX)
      .def_readwrite("uds_settings", &channel::Settings::uds_settings)
#endif
      .def_readwrite("shm_settings", &channel::Settings::shm_settings);
}

}  // namespace felicia