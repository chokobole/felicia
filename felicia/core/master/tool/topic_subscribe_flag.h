// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MASTER_TOOL_TOPIC_SUBSCRIBE_FLAG_H_
#define FELICIA_CORE_MASTER_TOOL_TOPIC_SUBSCRIBE_FLAG_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class TopicSubscribeFlag : public FlagParser::Delegate {
 public:
  TopicSubscribeFlag();
  ~TopicSubscribeFlag();

  const BoolFlag* all_flag() const { return all_flag_.get(); }
  const StringFlag* topic_flag() const { return topic_flag_.get(); }
  const Flag<uint32_t>* period_flag() const { return period_flag_.get(); }
  const Flag<uint8_t>* queue_size_flag() const {
    return queue_size_flag_.get();
  }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  bool all_;
  std::string topic_;
  uint32_t period_;
  uint8_t queue_size_;
  std::unique_ptr<BoolFlag> all_flag_;
  std::unique_ptr<StringFlag> topic_flag_;
  std::unique_ptr<Flag<uint32_t>> period_flag_;
  std::unique_ptr<Flag<uint8_t>> queue_size_flag_;

  DISALLOW_COPY_AND_ASSIGN(TopicSubscribeFlag);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_TOPIC_SUBSCRIBE_FLAG_H_