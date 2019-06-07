#ifndef FELICIA_PYTHON_COMMAND_LINE_INTERFACE_PY_H_
#define FELICIA_PYTHON_COMMAND_LINE_INTERFACE_PY_H_

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "felicia/core/util/command_line_interface/flag.h"
#include "felicia/core/util/command_line_interface/flag_parser.h"

namespace py = pybind11;

namespace felicia {

class PyFlagParserDelegate : public FlagParser::Delegate {
 public:
  using FlagParser::Delegate::Delegate;

  bool Parse(FlagParser& parser) override {
    PYBIND11_OVERLOAD_PURE(
        bool,                 /* Return type */
        FlagParser::Delegate, /* Parent class */
        Parse, /* Name of function in C++ (must match Python name) */
        parser /* Argument(s) */
    );
  }

  bool Validate() const override {
    PYBIND11_OVERLOAD(
        bool,                 /* Return type */
        FlagParser::Delegate, /* Parent class */
        Validate, /* Name of function in C++ (must match Python name) */
    );
  }

  std::vector<std::string> CollectUsages() const override {
    PYBIND11_OVERLOAD(
        std::vector<std::string>, /* Return type */
        FlagParser::Delegate,     /* Parent class */
        CollectUsages, /* Name of function in C++ (must match Python name) */
    );
  }

  std::string Description() const override {
    PYBIND11_OVERLOAD(
        std::string,          /* Return type */
        FlagParser::Delegate, /* Parent class */
        Description, /* Name of function in C++ (must match Python name) */
    );
  }

  std::vector<NamedHelpType> CollectNamedHelps() const override {
    PYBIND11_OVERLOAD(std::vector<NamedHelpType>, /* Return type */
                      FlagParser::Delegate,       /* Parent class */
                      CollectNamedHelps, /* Name of function in C++ (must
                                            match Python name) */
    );
  }
};

void AddCommandLineInterface(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_COMMAND_LINE_INTERFACE_PY_H_