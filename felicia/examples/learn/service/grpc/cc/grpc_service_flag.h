#ifndef FELICIA_EXAMPLE_LEARN_SERVICE_GRPC_CC_GRPC_SERVICE_FLAG_H_
#define FELICIA_EXAMPLE_LEARN_SERVICE_GRPC_CC_GRPC_SERVICE_FLAG_H_

#include "felicia/examples/learn/service/common/cc/service_create_flag.h"

namespace felicia {

class GrpcServiceFlag : public ServiceCreateFlag {
 public:
  GrpcServiceFlag();
  ~GrpcServiceFlag();

  const IntFlag* a_flag() const { return a_flag_.get(); }
  const IntFlag* b_flag() const { return b_flag_.get(); }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(name_flag_, is_server_flag_,
                                          service_flag_, a_flag_, b_flag_)

 private:
  int a_;
  int b_;
  std::unique_ptr<IntFlag> a_flag_;
  std::unique_ptr<IntFlag> b_flag_;

  DISALLOW_COPY_AND_ASSIGN(GrpcServiceFlag);
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLE_LEARN_SERVICE_GRPC_CC_GRPC_SERVICE_FLAG_H_