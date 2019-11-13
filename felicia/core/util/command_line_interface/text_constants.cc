// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/util/command_line_interface/text_constants.h"

namespace felicia {

const std::string kRedError = TextStyle::Red("Error: ");
const std::string kYellowWarning = TextStyle::Yellow("Warning: ");
const std::string kYellowOptions = TextStyle::Yellow("Options: ");
const std::string kYellowCommands = TextStyle::Yellow("Commands: ");

}  // namespace felicia