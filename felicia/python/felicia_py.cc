#include "pybind11/operators.h"
#include "pybind11/pybind11.h"

#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/felicia_init.h"
#include "felicia/python/command_line_interface/flag_py.h"
#include "felicia/python/master_proxy_py.h"
#include "felicia/python/node_py.h"

namespace py = pybind11;

namespace felicia {

extern void AddCommunication(py::module& m);

void AddGlobalFunctions(py::module& m) {
  m.def("felicia_init", &FeliciaInit);

  m.def("from_days", &::base::TimeDelta::FromDays)
      .def("from_hours", &::base::TimeDelta::FromHours)
      .def("from_minutes", &::base::TimeDelta::FromMinutes)
      .def("from_seconds", &::base::TimeDelta::FromSecondsD)
      .def("from_milliseconds", &::base::TimeDelta::FromMillisecondsD)
      .def("from_microseconds", &::base::TimeDelta::FromMicrosecondsD)
      .def("from_nanosecods", &::base::TimeDelta::FromNanosecondsD);

  m.def("unix_epoch", &::base::Time::UnixEpoch,
        "Returns the time for epoch in Unix-like system (Jan 1, 1970).")
      .def("now", &::base::Time::Now,
           "Returns the current time. Watch out, the system might adjust its "
           "clock in which case time will actually go backwards. We don't "
           "guarantee that times are increasing, or that two calls to Now() "
           "won't be the same.")
      .def("now_from_system_time", &::base::Time::NowFromSystemTime,
           "Returns the current time. Same as Now() except that this function "
           "always uses system time so that there are no discrepancies between"
           "the returned time and system time even on virtual environments "
           "including our test bot. or timing sensitive unittests, this "
           "function should be used.")
      .def("from_double_t", &::base::Time::FromDoubleT);
}

void AddGlobalObject(py::module& m) {
  py::class_<Status>(m, "Status")
      .def(py::init<>(), "Create a success status.")
      .def_static("OK", &Status::OK, "Convenience static method.")
      .def("error_code", &Status::error_code)
      .def("error_message", &Status::error_message)
      .def("ok", &Status::ok)
      .def("__repr__", &Status::error_message);

  py::class_<::base::TimeDelta>(m, "TimeDelta")
      .def(py::init<>())
      .def("in_days", &::base::TimeDelta::InDays)
      .def("in_days_floored", &::base::TimeDelta::InDaysFloored)
      .def("in_hours", &::base::TimeDelta::InHours)
      .def("in_minutes", &::base::TimeDelta::InMinutes)
      .def("in_seconds_f", &::base::TimeDelta::InSecondsF)
      .def("in_seconds", &::base::TimeDelta::InSeconds)
      .def("in_milliseconds_f", &::base::TimeDelta::InMicrosecondsF)
      .def("in_milliseconds", &::base::TimeDelta::InMicroseconds)
      .def("in_milliseconds_rounded_up",
           &::base::TimeDelta::InMillisecondsRoundedUp)
      .def("in_microseconds", &::base::TimeDelta::InMicroseconds)
      .def("in_microseconds_f", &::base::TimeDelta::InMicrosecondsF)
      .def("in_nanoseconds", &::base::TimeDelta::InNanoseconds)
      .def(py::self + py::self)
      .def(py::self - py::self)
      .def(py::self += py::self)
      .def(-py::self)
      .def(double() * py::self)
      .def(py::self * double())
      .def(py::self / py::self)
      .def(py::self % py::self)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__", [](const ::base::TimeDelta& self) {
        return ::base::StrCat(
            {::base::NumberToString(self.InMilliseconds()), "ms"});
      });

  py::class_<::base::Time>(m, "Time")
      .def(py::init<>(),
           "Contains the NULL time. Use Time::Now() to get the current time.")
      .def("to_double_t", &::base::Time::ToDoubleT)
      .def("__add__",
           [](const ::base::Time& time, const ::base::TimeDelta& delta) {
             return time + delta;
           })
      .def("__sub__",
           [](const ::base::Time& time, const ::base::TimeDelta& delta) {
             return time - delta;
           })
      .def("__iadd__",
           [](::base::Time& time, const ::base::TimeDelta& delta) {
             return time += delta;
           })
      .def("__isub__",
           [](::base::Time& time, const ::base::TimeDelta& delta) {
             return time -= delta;
           })
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__", [](const ::base::Time& self) {
        return ::base::StrCat({::base::NumberToString(self.ToDoubleT()), "s"});
      });
}

PYBIND11_MODULE(felicia_py, m) {
  m.doc() = "Bindings for Felicia.";

  AddGlobalFunctions(m);
  AddGlobalObject(m);
  AddMasterProxy(m);
  AddNodeLifecycle(m);
  AddCommunication(m);

  py::module command_line_interface = m.def_submodule("command_line_interface");
  AddFlag(command_line_interface);
}

}  // namespace felicia