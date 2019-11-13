// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MASTER_TOOL_CLIENT_LIST_FLAG_H_
#define FELICIA_CORE_MASTER_TOOL_CLIENT_LIST_FLAG_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class ClientListFlag : public FlagParser::Delegate {
 public:
  ClientListFlag();
  ~ClientListFlag();

  const BoolFlag* all_flag() const { return all_flag_.get(); }
  const Flag<uint32_t>* id_flag() const { return id_flag_.get(); }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  bool all_;
  uint32_t id_;
  std::unique_ptr<BoolFlag> all_flag_;
  std::unique_ptr<Flag<uint32_t>> id_flag_;

  DISALLOW_COPY_AND_ASSIGN(ClientListFlag);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_CLIENT_LIST_FLAG_H_