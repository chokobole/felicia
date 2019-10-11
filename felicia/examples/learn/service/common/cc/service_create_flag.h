#ifndef FELICIA_EXAMPLES_LEARN_SERVICE_COMMON_CC_SERVICE_CREATE_FLAG_H_
#define FELICIA_EXAMPLES_LEARN_SERVICE_COMMON_CC_SERVICE_CREATE_FLAG_H_

#include "felicia/examples/learn/common/cc/node_create_flag.h"

namespace felicia {

class ServiceCreateFlag : public NodeCreateFlag {
 public:
  ServiceCreateFlag();
  ~ServiceCreateFlag();

  const BoolFlag* is_server_flag() const { return is_server_flag_.get(); }
  const StringFlag* service_flag() const { return service_flag_.get(); }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(name_flag_, is_server_flag_,
                                          service_flag_)

 protected:
  bool is_server_;
  std::string service_;
  std::unique_ptr<BoolFlag> is_server_flag_;
  std::unique_ptr<StringFlag> service_flag_;

  DISALLOW_COPY_AND_ASSIGN(ServiceCreateFlag);
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_SERVICE_COMMON_CC_SERVICE_CREATE_FLAG_H_