#ifndef FELICIA_EXAMPLES_SLAM_ORB_SLAM2_FLAG_H_
#define FELICIA_EXAMPLES_SLAM_ORB_SLAM2_FLAG_H_

#include <memory>

#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class OrbSlam2Flag : public FlagParser::Delegate {
 public:
  OrbSlam2Flag();
  ~OrbSlam2Flag();

  const StringFlag* voc_path_flag() const { return voc_path_flag_.get(); }
  const StringFlag* settings_path_flag() const {
    return settings_path_flag_.get();
  }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  std::string voc_path_;
  std::string settings_path_;
  std::unique_ptr<StringFlag> voc_path_flag_;
  std::unique_ptr<StringFlag> settings_path_flag_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_ORB_SLAM2_FLAG_H_