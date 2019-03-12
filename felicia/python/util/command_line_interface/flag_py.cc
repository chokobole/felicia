#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <type_traits>

#include "felicia/core/util/command_line_interface/flag.h"
#include "felicia/core/util/command_line_interface/flag_parser.h"
#include "felicia/python/util/command_line_interface/py_flag_parser_delegate.h"

namespace py = pybind11;

namespace felicia {

template <typename FlagTy>
void AddFlagBuilderCommonAttributes(py::class_<typename FlagTy::Builder>& cls) {
  cls.def("set_short_name", &FlagTy::Builder::SetShortName)
      .def("set_long_name", &FlagTy::Builder::SetLongName)
      .def("set_name", &FlagTy::Builder::SetName)
      .def("set_help", &FlagTy::Builder::SetHelp)
      .def("build", &FlagTy::Builder::Build);
}

template <typename FlagTy>
std::enable_if_t<
    std::is_same<typename FlagTy::traits_type,
                 InitValueTraits<typename FlagTy::value_type>>::value,
    void>
AddFlagBuilder(py::module& m, const char* name) {
  py::class_<typename FlagTy::Builder> cls(m, name);
  cls.def(py::init([]() {
    auto value = new typename FlagTy::value_type;
    return typename FlagTy::Builder(MakeValueStore(value));
  }));
  AddFlagBuilderCommonAttributes<FlagTy>(cls);
}

template <typename FlagTy>
std::enable_if_t<
    std::is_same<typename FlagTy::traits_type,
                 DefaultValueTraits<typename FlagTy::value_type>>::value &&
        !std::is_same<typename FlagTy::flag_type,
                      Range<typename FlagTy::value_type>>::value &&
        !std::is_same<typename FlagTy::flag_type,
                      Choices<typename FlagTy::value_type>>::value,
    void>
AddFlagBuilder(py::module& m, const char* name) {
  py::class_<typename FlagTy::Builder> cls(m, name);
  cls.def(py::init([](typename FlagTy::value_type default_value) {
    auto value = new typename FlagTy::value_type;
    return typename FlagTy::Builder(MakeValueStore(value, default_value));
  }));
  AddFlagBuilderCommonAttributes<FlagTy>(cls);
}

template <typename FlagTy>
std::enable_if_t<
    std::is_same<typename FlagTy::traits_type,
                 DefaultValueTraits<typename FlagTy::value_type>>::value &&
        std::is_same<typename FlagTy::flag_type,
                     Range<typename FlagTy::value_type>>::value,
    void>
AddFlagBuilder(py::module& m, const char* name) {
  py::class_<typename FlagTy::Builder> cls(m, name);
  cls.def(py::init([](typename FlagTy::value_type default_value,
                      const std::pair<typename FlagTy::value_type,
                                      typename FlagTy::value_type>& pair) {
    auto value = new typename FlagTy::value_type;
    Range<typename FlagTy::value_type> range{pair.first, pair.second};
    return
        typename FlagTy::Builder(MakeValueStore(value, default_value, range));
  }));
  AddFlagBuilderCommonAttributes<FlagTy>(cls);
}

template <typename FlagTy>
std::enable_if_t<
    std::is_same<typename FlagTy::traits_type,
                 DefaultValueTraits<typename FlagTy::value_type>>::value &&
        std::is_same<typename FlagTy::flag_type,
                     Choices<typename FlagTy::value_type>>::value,
    void>
AddFlagBuilder(py::module& m, const char* name) {
  py::class_<typename FlagTy::Builder> cls(m, name);
  cls.def(py::init([](typename FlagTy::value_type default_value,
                      const std::vector<typename FlagTy::value_type> vecs) {
    auto value = new typename FlagTy::value_type;
    Choices<typename FlagTy::value_type> choices{vecs};
    return
        typename FlagTy::Builder(MakeValueStore(value, default_value, choices));
  }));
  AddFlagBuilderCommonAttributes<FlagTy>(cls);
}

template <typename FlagTy>
void AddFlag(py::module& m, const char* name) {
  py::class_<FlagTy>(m, name)
      .def("short_name", &FlagTy::short_name)
      .def("long_name", &FlagTy::long_name)
      .def("name", &FlagTy::name)
      .def("usage", &FlagTy::usage)
      .def("help", &FlagTy::help, py::arg("help_start") = 20)
      .def("is_positional", &FlagTy::is_positional)
      .def("is_optional", &FlagTy::is_optional)
      .def("value", &FlagTy::value)
      .def("is_set", &FlagTy::is_set)
      .def("release", &FlagTy::release)
      .def("parse", &FlagTy::Parse);
}

PYBIND11_MODULE(flag, m) {
  m.doc() = "Bindings for flag.";

#define ADD_FLAG(Flag)                      \
  AddFlagBuilder<Flag>(m, #Flag "Builder"); \
  AddFlag<Flag>(m, #Flag)

  ADD_FLAG(BoolFlag);
  ADD_FLAG(IntFlag);
  ADD_FLAG(DoubleFlag);
  ADD_FLAG(StringFlag);
  ADD_FLAG(BoolDefaultFlag);
  ADD_FLAG(IntDefaultFlag);
  ADD_FLAG(DoubleDefaultFlag);
  ADD_FLAG(StringDefaultFlag);
  ADD_FLAG(IntRangeFlag);
  ADD_FLAG(DoubleRangeFlag);
  ADD_FLAG(StringRangeFlag);
  ADD_FLAG(IntChoicesFlag);
  ADD_FLAG(DoubleChoicesFlag);
  ADD_FLAG(StringChoicesFlag);

#undef ADD_FLAG

  py::class_<FlagParser::Delegate, PyFlagParserDelegate>(m,
                                                         "FlagParserDelegate")
      .def(py::init<>())
      .def("Parse", &FlagParser::Delegate::Parse)
      .def("Validate", &FlagParser::Delegate::Validate)
      .def("CollectUsages", &FlagParser::Delegate::CollectUsages)
      .def("Description", &FlagParser::Delegate::Description)
      .def("CollectNamedHelps", &FlagParser::Delegate::CollectNamedHelps);

  py::class_<FlagParser>(m, "FlagParser")
      .def(py::init<>())
      .def("set_program_name", &FlagParser::set_program_name,
           "Set program name to display.\n"
           "For example, when there are --foo, --bar flags and type --help\n"
           "It shows |program_name| [--foo] [--bar]")
      .def(
          "parse",
          [](FlagParser& self, int argc, std::vector<std::string> argv,
             FlagParser::Delegate& delegate) {
            char* tmp_argv[argc];
            for (int i = 0; i < argc; i++) {
              tmp_argv[i] = const_cast<char*>(argv[i].c_str());
            }
            return self.Parse(argc, tmp_argv, &delegate);
          },
          "// Parse by passing every each |argv| to |flag|.");
}

}  // namespace felicia
