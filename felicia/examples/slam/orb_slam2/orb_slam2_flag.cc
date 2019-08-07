#include "felicia/examples/slam/orb_slam2/orb_slam2_flag.h"

namespace felicia {
namespace orb_slam2 {

OrbSlam2Flag::OrbSlam2Flag() {
  {
    StringFlag::Builder builder(MakeValueStore(&voc_path_));
    auto flag = builder.SetLongName("--voc_path")
                    .SetHelp("Path to vocabulary file.")
                    .Build();
    voc_path_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&settings_path_));
    auto flag = builder.SetLongName("--settings_path")
                    .SetHelp("Path to vocabulary file.")
                    .Build();
    settings_path_flag_ = std::make_unique<StringFlag>(flag);
  }
}

OrbSlam2Flag::~OrbSlam2Flag() = default;

bool OrbSlam2Flag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, voc_path_flag_, settings_path_flag_);
}

bool OrbSlam2Flag::Validate() const {
  return voc_path_flag_->is_set() && settings_path_flag_->is_set();
}

std::vector<std::string> OrbSlam2Flag::CollectUsages() const {
  std::vector<std::string> usages;
  usages.push_back("[--help]");
  AddUsage(usages, voc_path_flag_, settings_path_flag_);
  return usages;
}

std::string OrbSlam2Flag::Description() const { return "ORB-SLAM2 flags"; }

std::vector<NamedHelpType> OrbSlam2Flag::CollectNamedHelps() const {
  return {
      std::make_pair(kYellowOptions,
                     std::vector<std::string>{voc_path_flag_->help(),
                                              settings_path_flag_->help()}),
  };
}

}  // namespace orb_slam2
}  // namespace felicia