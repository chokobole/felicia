#include "felicia/examples/learn/service/common/cc/service_create_flag.h"

namespace felicia {

ServiceCreateFlag::ServiceCreateFlag() {
  {
    BoolFlag::Builder builder(MakeValueStore(&is_server_));
    auto flag = builder.SetLongName("--server")
                    .SetHelp(
                        "create server node, if set, default: false(create "
                        "client node)")
                    .Build();
    is_server_flag_ = std::make_unique<BoolFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&service_));
    auto flag = builder.SetShortName("-s")
                    .SetLongName("--service")
                    .SetHelp("name for service, it's required to be set")
                    .Build();
    service_flag_ = std::make_unique<StringFlag>(flag);
  }
}

ServiceCreateFlag::~ServiceCreateFlag() = default;

bool ServiceCreateFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, name_flag_, is_server_flag_,
                             service_flag_);
}

bool ServiceCreateFlag::Validate() const {
  return CheckIfFlagWasSet(service_flag_);
}

}  // namespace felicia