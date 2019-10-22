
#include "pybind11/operators.h"
#include "pybind11/pybind11.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/utf_string_conversions.h"

#include "felicia/core/communication/settings.h"
#include "felicia/core/felicia_init.h"
#include "felicia/core/util/timestamp/timestamper.h"
#include "felicia/python/channel/channel_py.h"
#include "felicia/python/command_line_interface/command_line_interface_py.h"
#include "felicia/python/communication/communication_py.h"
#include "felicia/python/drivers/drivers_py.h"
#include "felicia/python/master/master_proxy_py.h"
#include "felicia/python/node/node_lifecycle_py.h"
#include "felicia/python/rpc/rpc_py.h"
#include "felicia/python/type_conversion/protobuf.h"

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
      .def(py::init<error::Code, const std::string&>(), py::arg("error_code"),
           py::arg("error_message"))
      .def_static("OK", &Status::OK, "Convenience static method.")
      .def("error_code", &Status::error_code)
      .def("error_message", [](Status& self) { return self.error_message(); })
      .def("ok", &Status::ok)
      .def("__repr__", [](Status& self) { return self.error_message(); });

  py::class_<base::TimeDelta>(m, "TimeDelta")
      .def(py::init<>())
      .def_static("from_days", &base::TimeDelta::FromDays, py::arg("days"))
      .def_static("from_hours", &base::TimeDelta::FromHours, py::arg("hours"))
      .def_static("from_minutes", &base::TimeDelta::FromMinutes,
                  py::arg("minutes"))
      .def_static("from_seconds", &base::TimeDelta::FromSeconds,
                  py::arg("secs"))
      .def_static("from_milliseconds", &base::TimeDelta::FromMilliseconds,
                  py::arg("ms"))
      .def_static("from_microseconds", &base::TimeDelta::FromMicroseconds,
                  py::arg("us"))
      .def_static("from_nanosecods", &base::TimeDelta::FromNanoseconds,
                  py::arg("ns"))
      .def_static("from_seconds_d", &base::TimeDelta::FromSecondsD,
                  py::arg("secs"))
      .def_static("from_milliseconds_d", &base::TimeDelta::FromMillisecondsD,
                  py::arg("ms"))
      .def_static("from_microseconds_d", &base::TimeDelta::FromMicrosecondsD,
                  py::arg("us"))
      .def_static("from_nanosecods_d", &base::TimeDelta::FromNanosecondsD,
                  py::arg("ns"))
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
          "now", &base::Time::Now,
          "Returns the current time. Watch out, the system might adjust its "
          "clock in which case time will actually go backwards. We don't "
          "guarantee that times are increasing, or that two calls to Now() "
          "won't be the same.")
      .def("__add__",
           [](const base::Time& time, const base::TimeDelta& delta) {
             return time + delta;
           },
           py::arg("delta"))
      .def("__sub__",
           [](const base::Time& time, const base::TimeDelta& delta) {
             return time - delta;
           },
           py::arg("delta"))
      .def("__iadd__",
           [](base::Time& time, const base::TimeDelta& delta) {
             return time += delta;
           },
           py::arg("delta"))
      .def("__isub__",
           [](base::Time& time, const base::TimeDelta& delta) {
             return time -= delta;
           },
           py::arg("delta"))
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
      .def_static("from_bytes", &Bytes::FromBytes, py::arg("bytes"))
      .def_static("from_killo_bytes", &Bytes::FromKilloBytes,
                  py::arg("killo_bytes"))
      .def_static("from_killo_bytes_d", &Bytes::FromKilloBytesD,
                  py::arg("killo_bytes"))
      .def_static("from_mega_bytes", &Bytes::FromMegaBytes,
                  py::arg("mega_bytes"))
      .def_static("from_mega_bytes_d", &Bytes::FromMegaBytesD,
                  py::arg("mega_bytes"))
      .def_static("from_giga_bytes", &Bytes::FromGigaBytes,
                  py::arg("giga_bytes"))
      .def_static("from_giga_bytes_d", &Bytes::FromGigaBytesD,
                  py::arg("giga_bytes"))
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

  py::class_<base::FilePath>(m, "FilePath")
      .def(py::init<>())
      .def(py::init([](const std::string& path) {
#if defined(OS_WIN)
             return base::FilePath(base::UTF8ToUTF16(path));
#else
             return base::FilePath(path);
#endif
           }),
           py::arg("path"))
      .def("__str__", &base::FilePath::AsUTF8Unsafe);

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

  AddChannel(m);
  AddCommunication(m);
  AddCommandLineInterface(m);
  AddDrivers(m);
  AddRpc(m);
  AddMasterProxy(m);
  AddNodeLifecycle(m);
}

}  // namespace felicia