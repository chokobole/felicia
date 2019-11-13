// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/python/command_line_interface/command_line_interface_py.h"

#include "felicia/python/command_line_interface/flag_py.h"

namespace felicia {

void AddCommandLineInterface(py::module& m) {
  py::module command_line_interface = m.def_submodule("command_line_interface");

  AddFlag(command_line_interface);

  py::class_<TextStyle>(command_line_interface, "TextStyle")
      .def_static("red",
                  [](const std::string& text) { return TextStyle::Red(text); },
                  py::arg("text"))
      .def_static(
          "green",
          [](const std::string& text) { return TextStyle::Green(text); },
          py::arg("text"))
      .def_static("blue",
                  [](const std::string& text) { return TextStyle::Blue(text); },
                  py::arg("text"))
      .def_static(
          "yellow",
          [](const std::string& text) { return TextStyle::Yellow(text); },
          py::arg("text"))
      .def_static("bold",
                  [](const std::string& text) { return TextStyle::Bold(text); },
                  py::arg("text"));

  command_line_interface.attr("RED_ERROR") = kRedError;
  command_line_interface.attr("YELLOW_OPTIONS") = kYellowOptions;
  command_line_interface.attr("YELLOW_COMMANDS") = kYellowCommands;
}

}  // namespace felicia