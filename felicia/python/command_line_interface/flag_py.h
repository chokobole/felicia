#ifndef FELICIA_PYTHON_COMMAND_LINE_INTERFACE_FLAG_PY_H_
#define FELICIA_PYTHON_COMMAND_LINE_INTERFACE_FLAG_PY_H_

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "felicia/core/util/command_line_interface/flag.h"
#include "felicia/core/util/command_line_interface/flag_parser.h"
#include "felicia/python/type_conversion/util.h"

namespace py = pybind11;

namespace felicia {

class PyFlagParserDelegate : public FlagParser::Delegate {
 public:
  using FlagParser::Delegate::Delegate;

  bool Parse(FlagParser& parser) override {
    PYBIND11_OVERLOAD_INT(bool, FlagParser::Delegate, "parse", parser);
    FEL_CALL_PURE_FUNCTION(FlagParser::Delegate, "Parse");
  }

  bool Validate() const override {
    PYBIND11_OVERLOAD_INT(bool, FlagParser::Delegate, "validate");
    return FlagParser::Delegate::Validate();
  }

  std::vector<std::string> CollectUsages() const override {
    PYBIND11_OVERLOAD_INT(std::vector<std::string>, FlagParser::Delegate,
                          "collect_usages");
    return FlagParser::Delegate::CollectUsages();
  }

  std::string Description() const override {
    PYBIND11_OVERLOAD_INT(std::string, FlagParser::Delegate, "description");
    return FlagParser::Delegate::Description();
  }

  std::vector<NamedHelpType> CollectNamedHelps() const override {
    PYBIND11_OVERLOAD_INT(std::vector<NamedHelpType>, FlagParser::Delegate,
                          "collect_named_helps");
    return FlagParser::Delegate::CollectNamedHelps();
  }
};

void AddFlag(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_COMMAND_LINE_INTERFACE_FLAG_PY_H_