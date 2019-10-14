
#include "pybind11/operators.h"
#include "pybind11/pybind11.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/communication/settings.h"
#include "felicia/core/felicia_init.h"
#include "felicia/core/util/timestamp/timestamper.h"
#include "felicia/python/channel_py.h"
#include "felicia/python/command_line_interface_py.h"
#include "felicia/python/communication_py.h"
#include "felicia/python/drivers_py.h"
#include "felicia/python/master_proxy_py.h"
#include "felicia/python/node_py.h"
#include "felicia/python/rpc_py.h"

namespace py = pybind11;

namespace felicia {

void AddGlobalFunctions(py::module& m) {
  m.def("felicia_init", &FeliciaInit);

  m.attr("INFO") = ::logging::LOG_INFO;
  m.attr("WARNING") = ::logging::LOG_WARNING;
  m.attr("ERROR") = ::logging::LOG_ERROR;
  m.attr("FATAL") = ::logging::LOG_FATAL;

  m.def("log",
        [](::logging::LogSeverity severity, const std::string& text) {
          if (severity == ::logging::LOG_INFO)
            LOG(INFO) << text;
          else if (severity == ::logging::LOG_WARNING)
            LOG(WARNING) << text;
          else if (severity == ::logging::LOG_ERROR)
            LOG(ERROR) << text;
          else if (severity == ::logging::LOG_FATAL)
            LOG(FATAL) << text;
        },
        py::arg("severity"), py::arg("text") = base::EmptyString())
      .def("log_if",
           [](::logging::LogSeverity severity, bool condition,
              const std::string& text) {
             if (severity == ::logging::LOG_INFO)
               LOG_IF(INFO, condition) << text;
             else if (severity == ::logging::LOG_WARNING)
               LOG_IF(WARNING, condition) << text;
             else if (severity == ::logging::LOG_ERROR)
               LOG_IF(ERROR, condition) << text;
             else if (severity == ::logging::LOG_FATAL)
               LOG_IF(FATAL, condition) << text;
           },
           py::arg("severity"), py::arg("condition"),
           py::arg("text") = base::EmptyString())
      .def("check",
           [](bool condition, const std::string& text) {
             CHECK(condition) << text;
           },
           py::arg("condition"), py::arg("text") = base::EmptyString())
      .def("not_reached", []() { NOTREACHED(); });
}

void AddGlobalObject(py::module& m) {
  py::class_<Status>(m, "Status")
      .def(py::init<>(), "Create a success status.")
      .def_static("OK", &Status::OK, "Convenience static method.")
      .def("error_code", &Status::error_code)
      .def("error_message", &Status::error_message)
      .def("ok", &Status::ok)
      .def("__repr__", &Status::error_message);

  py::class_<base::TimeDelta>(m, "TimeDelta")
      .def(py::init<>())
      .def_static("from_days", &base::TimeDelta::FromDays)
      .def_static("from_hours", &base::TimeDelta::FromHours)
      .def_static("from_minutes", &base::TimeDelta::FromMinutes)
      .def_static("from_seconds", &base::TimeDelta::FromSecondsD)
      .def_static("from_milliseconds", &base::TimeDelta::FromMillisecondsD)
      .def_static("from_microseconds", &base::TimeDelta::FromMicrosecondsD)
      .def_static("from_nanosecods", &base::TimeDelta::FromNanosecondsD)
      .def("in_days", &base::TimeDelta::InDays)
      .def("in_days_floored", &base::TimeDelta::InDaysFloored)
      .def("in_hours", &base::TimeDelta::InHours)
      .def("in_minutes", &base::TimeDelta::InMinutes)
      .def("in_seconds_f", &base::TimeDelta::InSecondsF)
      .def("in_seconds", &base::TimeDelta::InSeconds)
      .def("in_milliseconds_f", &base::TimeDelta::InMicrosecondsF)
      .def("in_milliseconds", &base::TimeDelta::InMicroseconds)
      .def("in_milliseconds_rounded_up",
           &base::TimeDelta::InMillisecondsRoundedUp)
      .def("in_microseconds", &base::TimeDelta::InMicroseconds)
      .def("in_microseconds_f", &base::TimeDelta::InMicrosecondsF)
      .def("in_nanoseconds", &base::TimeDelta::InNanoseconds)
      .def(py::self + py::self)
      .def(py::self - py::self)
      .def(py::self += py::self)
      .def(py::self -= py::self)
      .def(-py::self)
      .def(double() * py::self)
      .def(py::self * double())
      .def(py::self *= double())
      .def(py::self / double())
      .def(py::self /= double())
      .def(py::self / py::self)
      .def(py::self % py::self)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__", [](const base::TimeDelta& self) {
        std::stringstream ss;
        ss << self;
        return ss.str();
      });

  py::class_<base::Time>(m, "Time")
      .def(py::init<>(),
           "Contains the NULL time. Use Time::Now() to get the current time.")
      .def_static(
          "unix_epoch", &base::Time::UnixEpoch,
          "Returns the time for epoch in Unix-like system (Jan 1, 1970).")
      .def_static(
          "now", &base::Time::Now,
          "Returns the current time. Watch out, the system might adjust its "
          "clock in which case time will actually go backwards. We don't "
          "guarantee that times are increasing, or that two calls to Now() "
          "won't be the same.")
      .def_static(
          "now_from_system_time", &base::Time::NowFromSystemTime,
          "Returns the current time. Same as Now() except that this function "
          "always uses system time so that there are no discrepancies between"
          "the returned time and system time even on virtual environments "
          "including our test bot. or timing sensitive unittests, this "
          "function should be used.")
      .def_static("from_double_t", &base::Time::FromDoubleT)
      .def("to_double_t", &base::Time::ToDoubleT)
      .def("__add__", [](const base::Time& time,
                         const base::TimeDelta& delta) { return time + delta; })
      .def("__sub__", [](const base::Time& time,
                         const base::TimeDelta& delta) { return time - delta; })
      .def("__iadd__",
           [](base::Time& time, const base::TimeDelta& delta) {
             return time += delta;
           })
      .def("__isub__",
           [](base::Time& time, const base::TimeDelta& delta) {
             return time -= delta;
           })
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__", [](const base::Time& self) {
        std::stringstream ss;
        ss << self;
        return ss.str();
      });

  py::class_<Bytes>(m, "Bytes")
      .def(py::init<>())
      .def_static("from_bytes", &Bytes::FromBytes)
      .def_static("from_killo_bytes", &Bytes::FromKilloBytes)
      .def_static("from_killo_bytes_d", &Bytes::FromKilloBytesD)
      .def_static("from_mega_bytes", &Bytes::FromMegaBytes)
      .def_static("from_mega_bytes_d", &Bytes::FromMegaBytesD)
      .def_static("from_giga_bytes", &Bytes::FromGigaBytes)
      .def_static("from_giga_bytes_d", &Bytes::FromGigaBytesD)
      .def("bytes", &Bytes::bytes)
      .def(py::self + py::self)
      .def(py::self - py::self)
      .def(py::self += py::self)
      .def(py::self -= py::self)
      .def(double() * py::self)
      .def(py::self * double())
      .def(py::self *= double())
      .def(py::self / double())
      .def(py::self /= double())
      .def(py::self / py::self)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__", [](const Bytes& self) {
        std::stringstream ss;
        ss << self;
        return ss.str();
      });

  py::class_<Timestamper>(m, "Timestamper")
      .def(py::init<>())
      .def("timestamp", &Timestamper::timestamp);

  py::class_<ThreadSafeTimestamper>(m, "ThreadSafeTimestamper")
      .def(py::init<>())
      .def("timestamp", &ThreadSafeTimestamper::timestamp);
}

PYBIND11_MODULE(felicia_py, m) {
  m.doc() = "Bindings for Felicia.";

  AddGlobalFunctions(m);
  AddGlobalObject(m);
  AddMasterProxy(m);
  AddNodeLifecycle(m);
  AddChannel(m);
  AddCommunication(m);
  AddCommandLineInterface(m);
  rpc::AddRpc(m);
  drivers::AddDrivers(m);
}

}  // namespace felicia