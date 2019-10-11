#include "felicia/examples/learn/service/grpc/cc/grpc_service_flag.h"

namespace felicia {

GrpcServiceFlag::GrpcServiceFlag() {
  {
    IntFlag::Builder builder(MakeValueStore(&a_));
    auto flag = builder.SetShortName("-a").SetHelp("number for a").Build();
    a_flag_ = std::make_unique<IntFlag>(flag);
  }
  {
    IntFlag::Builder builder(MakeValueStore(&b_));
    auto flag = builder.SetShortName("-b").SetHelp("number for b").Build();
    b_flag_ = std::make_unique<IntFlag>(flag);
  }
}

GrpcServiceFlag::~GrpcServiceFlag() = default;

bool GrpcServiceFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, name_flag_, is_server_flag_, service_flag_,
                             a_flag_, b_flag_);
}

bool GrpcServiceFlag::Validate() const {
  if (is_server_flag_->value()) {
    if (a_flag_->is_set() || b_flag_->is_set()) {
      std::cerr
          << kRedError
          << "Either a or b was set. but if it's server, it will be ignored."
          << std::endl;
    }
  }
  return CheckIfFlagWasSet(service_flag_);
}

}  // namespace felicia