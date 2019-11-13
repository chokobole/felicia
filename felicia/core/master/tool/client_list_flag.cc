// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/master/tool/client_list_flag.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

ClientListFlag::ClientListFlag() {
  {
    BoolFlag::Builder builder(MakeValueStore(&all_));
    auto flag = builder.SetShortName("-a")
                    .SetLongName("--all")
                    .SetHelp("List all the clients")
                    .Build();
    all_flag_ = std::make_unique<BoolFlag>(flag);
  }
  {
    Flag<uint32_t>::Builder builder(MakeValueStore(&id_));
    auto flag = builder.SetLongName("--id")
                    .SetHelp("List clients with a given id")
                    .Build();
    id_flag_ = std::make_unique<Flag<uint32_t>>(flag);
  }
}

ClientListFlag::~ClientListFlag() = default;

bool ClientListFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, all_flag_, id_flag_);
}

bool ClientListFlag::Validate() const {
  return CheckIfOneOfFlagWasSet(all_flag_, id_flag_);
}

std::vector<std::string> ClientListFlag::CollectUsages() const {
  return {"[OPTIONS]"};
}

std::string ClientListFlag::Description() const { return "List clients"; }

std::vector<NamedHelpType> ClientListFlag::CollectNamedHelps() const {
  return {
      std::make_pair(kYellowOptions,
                     std::vector<std::string>{
                         all_flag_->help(),
                         id_flag_->help(),
                     }),
  };
}

}  // namespace felicia