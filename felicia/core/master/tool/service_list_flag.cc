// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/master/tool/service_list_flag.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

ServiceListFlag::ServiceListFlag() {
  {
    BoolFlag::Builder builder(MakeValueStore(&all_));
    auto flag = builder.SetShortName("-a")
                    .SetLongName("--all")
                    .SetHelp("List all the services")
                    .Build();
    all_flag_ = std::make_unique<BoolFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&service_));
    auto flag = builder.SetShortName("-s")
                    .SetLongName("--service")
                    .SetHelp("List a given service")
                    .Build();
    service_flag_ = std::make_unique<StringFlag>(flag);
  }
}

ServiceListFlag::~ServiceListFlag() = default;

bool ServiceListFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, all_flag_, service_flag_);
}

bool ServiceListFlag::Validate() const {
  return CheckIfOneOfFlagWasSet(all_flag_, service_flag_);
}

std::vector<std::string> ServiceListFlag::CollectUsages() const {
  return {"[OPTIONS]"};
}

std::string ServiceListFlag::Description() const { return "List services"; }

std::vector<NamedHelpType> ServiceListFlag::CollectNamedHelps() const {
  return {
      std::make_pair(kYellowOptions,
                     std::vector<std::string>{
                         all_flag_->help(),
                         service_flag_->help(),
                     }),
  };
}

}  // namespace felicia