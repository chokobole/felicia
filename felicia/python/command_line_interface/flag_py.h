// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
    FEL_OVERLOAD_INT_WITHOUT_GIL(bool, FlagParser::Delegate, "parse", parser);
    FEL_CALL_PURE_FUNCTION(FlagParser::Delegate, "Parse");
  }

  bool Validate() const override {
    FEL_OVERLOAD_INT_WITHOUT_GIL(bool, FlagParser::Delegate, "validate");
    return FlagParser::Delegate::Validate();
  }

  std::vector<std::string> CollectUsages() const override {
    FEL_OVERLOAD_INT_WITHOUT_GIL(std::vector<std::string>, FlagParser::Delegate,
                                 "collect_usages");
    return FlagParser::Delegate::CollectUsages();
  }

  std::string Description() const override {
    FEL_OVERLOAD_INT_WITHOUT_GIL(std::string, FlagParser::Delegate,
                                 "description");
    return FlagParser::Delegate::Description();
  }

  std::vector<NamedHelpType> CollectNamedHelps() const override {
    FEL_OVERLOAD_INT_WITHOUT_GIL(std::vector<NamedHelpType>,
                                 FlagParser::Delegate, "collect_named_helps");
    return FlagParser::Delegate::CollectNamedHelps();
  }
};

void AddFlag(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_COMMAND_LINE_INTERFACE_FLAG_PY_H_