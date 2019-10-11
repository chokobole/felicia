#include "felicia/examples/learn/common/cc/node_create_flag.h"

namespace felicia {

NodeCreateFlag::NodeCreateFlag() {
  {
    StringFlag::Builder builder(MakeValueStore(&name_));
    auto flag = builder.SetShortName("-n")
                    .SetLongName("--name")
                    .SetHelp("name for node")
                    .Build();
    name_flag_ = std::make_unique<StringFlag>(flag);
  }
}

NodeCreateFlag::~NodeCreateFlag() = default;

bool NodeCreateFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, name_flag_);
}

}  // namespace felicia