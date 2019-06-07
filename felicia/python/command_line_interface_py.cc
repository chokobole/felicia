#include "felicia/python/command_line_interface_py.h"

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

void AddCommandLineInterface(py::module& m) {
  py::module command_line_interface = m.def_submodule("command_line_interface");

#define ADD_FLAG(Flag)                                           \
  AddFlagBuilder<Flag>(command_line_interface, #Flag "Builder"); \
  AddFlag<Flag>(command_line_interface, #Flag)

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

  py::class_<FlagParser::Delegate, PyFlagParserDelegate>(command_line_interface,
                                                         "_FlagParserDelegate")
      .def(py::init<>())
      .def("Parse", &FlagParser::Delegate::Parse)
      .def("Validate", &FlagParser::Delegate::Validate)
      .def("CollectUsages", &FlagParser::Delegate::CollectUsages)
      .def("Description", &FlagParser::Delegate::Description)
      .def("CollectNamedHelps", &FlagParser::Delegate::CollectNamedHelps);

  py::class_<TextStyle>(command_line_interface, "TextStyle")
      .def_static("red",
                  [](const std::string& text) { return TextStyle::Red(text); })
      .def_static(
          "green",
          [](const std::string& text) { return TextStyle::Green(text); })
      .def_static("blue",
                  [](const std::string& text) { return TextStyle::Blue(text); })
      .def_static(
          "yellow",
          [](const std::string& text) { return TextStyle::Yellow(text); })
      .def_static("bold", [](const std::string& text) {
        return TextStyle::Bold(text);
      });

  py::class_<FlagParser>(command_line_interface, "FlagParser")
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
          py::call_guard<py::gil_scoped_release>(),
          "Parse by passing every each |argv| to |flag|.");

  command_line_interface.attr("RED_ERROR") = kRedError;
  command_line_interface.attr("YELLOW_OPTIONS") = kYellowOptions;
  command_line_interface.attr("YELLOW_COMMANDS") = kYellowCommands;
}

}  // namespace felicia