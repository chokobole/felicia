// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/master/tool/topic_subscribe_flag.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

TopicSubscribeFlag::TopicSubscribeFlag() {
  {
    BoolFlag::Builder builder(MakeValueStore(&all_));
    auto flag = builder.SetShortName("-a")
                    .SetLongName("--all")
                    .SetHelp("Subscribe all the topcis")
                    .Build();
    all_flag_ = std::make_unique<BoolFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&topic_));
    auto flag = builder.SetShortName("-t")
                    .SetLongName("--topic")
                    .SetHelp("Topic to subscribe")
                    .Build();
    topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    Flag<uint32_t>::Builder builder(MakeValueStore(&period_));
    auto flag =
        builder.SetShortName("-i")
            .SetLongName("--interval")
            .SetHelp("Interval between messages in milliseconds, default: 1000")
            .Build();
    period_flag_ = std::make_unique<Flag<uint32_t>>(flag);
  }
  {
    Flag<uint8_t>::Builder builder(MakeValueStore(&queue_size_));
    auto flag = builder.SetShortName("-q")
                    .SetLongName("--queue_size")
                    .SetHelp("Queue size for each subsciber, default 10")
                    .Build();
    queue_size_flag_ = std::make_unique<Flag<uint8_t>>(flag);
  }
}

TopicSubscribeFlag::~TopicSubscribeFlag() = default;

bool TopicSubscribeFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, all_flag_, topic_flag_, period_flag_,
                             queue_size_flag_);
}

bool TopicSubscribeFlag::Validate() const {
  return CheckIfOneOfFlagWasSet(all_flag_, topic_flag_);
}

std::vector<std::string> TopicSubscribeFlag::CollectUsages() const {
  return {"[OPTIONS]"};
}

std::string TopicSubscribeFlag::Description() const {
  return "Subscribe topics";
}

std::vector<NamedHelpType> TopicSubscribeFlag::CollectNamedHelps() const {
  return {
      std::make_pair(kYellowOptions,
                     std::vector<std::string>{
                         all_flag_->help(),
                         topic_flag_->help(),
                         period_flag_->help(),
                         queue_size_flag_->help(),
                     }),
  };
}

}  // namespace felicia