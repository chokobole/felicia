#include "felicia/python/channel_py.h"

#include "felicia/core/channel/settings.h"
#include "felicia/core/channel/socket/ssl_server_context.h"
#include "felicia/python/type_conversion/callback.h"

namespace felicia {

void AddChannel(py::module& m) {
  py::module channel = m.def_submodule("channel");

  py::class_<SSLServerContext>(channel, "SSLServerContext")
      .def_static("new_ssl_server_context",
                  &SSLServerContext::NewSSLServerContext,
                  py::arg("cert_file_path"), py::arg("private_key_file_path"));

  py::class_<channel::TCPSettings>(channel, "TCPSettings")
      .def(py::init<>())
      .def_readwrite("use_ssl", &channel::TCPSettings::use_ssl)
      .def_readwrite("ssl_server_context",
                     &channel::TCPSettings::ssl_server_context);

#if defined(OS_POSIX)
  py::class_<UnixDomainServerSocket::Credentials>(channel, "Credentials")
      .def(py::init<>())
#if defined(OS_LINUX) || defined(OS_ANDROID) || defined(OS_FUCHSIA)
      .def_readwrite("process_id",
                     &UnixDomainServerSocket::Credentials::process_id)
#endif
      .def_readwrite("user_id", &UnixDomainServerSocket::Credentials::user_id)
      .def_readwrite("group_id",
                     &UnixDomainServerSocket::Credentials::group_id);

  using PyAuthCallback =
      PyCallback<bool(const UnixDomainServerSocket::Credentials&)>;

  py::class_<channel::UDSSettings>(channel, "UDSSettings")
      .def(py::init<>())
      .def_property("auth_callback",
                    [](channel::UDSSettings& self) {
                      PyErr_SetString(PyExc_TypeError,
                                      "Callback type can't be read from c++.");
                      throw py::error_already_set();
                    },
                    [](channel::UDSSettings& self, py::function auth_callback) {
                      self.auth_callback = base::BindRepeating(
                          &PyAuthCallback::Invoke,
                          base::Owned(new PyAuthCallback(auth_callback)));
                    },
                    py::arg("auth_callback"));
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