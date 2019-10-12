#ifndef FELICIA_CORE_MASTER_TOOL_SERVICE_FLAG_H_
#define FELICIA_CORE_MASTER_TOOL_SERVICE_FLAG_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/core/master/tool/service_list_flag.h"
#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class ServiceFlag : public FlagParser::Delegate {
 public:
  enum Command {
    COMMAND_SELF,
    COMMAND_LIST,
  };

  ServiceFlag();
  ~ServiceFlag();

  const ServiceListFlag& list_delegate() const { return list_delegate_; }
  Command command() const { return current_command_; }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  std::string command_;
  std::unique_ptr<StringChoicesFlag> command_flag_;
  ServiceListFlag list_delegate_;
  Command current_command_;

  DISALLOW_COPY_AND_ASSIGN(ServiceFlag);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOOL_SERVICE_FLAG_H_