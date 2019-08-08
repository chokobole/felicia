#include "felicia/examples/slam/slam_node_create_flag.h"

#include "felicia/core/util/command_line_interface/text_constants.h"

namespace felicia {

static const char* kHectorSlam = "HectorSlam";
static const char* kOrb2Slam = "OrbSlam2";

SlamNodeCreateFlag::SlamNodeCreateFlag() : current_slam_kind_(SLAM_KIND_NONE) {
  {
    StringChoicesFlag::Builder builder(
        MakeValueStore(&slam_kind_, ::base::EmptyString(),
                       Choices<std::string>{kHectorSlam, kOrb2Slam}));
    auto flag =
        builder.SetLongName("--slam_kind").SetHelp("slam kind to run").Build();
    slam_kind_flag_ = std::make_unique<StringChoicesFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&name_));
    auto flag = builder.SetShortName("-n")
                    .SetLongName("--name")
                    .SetHelp("name for node")
                    .Build();
    name_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&left_color_topic_));
    auto flag =
        builder.SetLongName("--left_color_topic")
            .SetHelp("topic to subscribe mono camera or left camera of stereo")
            .Build();
    left_color_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&right_color_topic_));
    auto flag = builder.SetLongName("--right_color_topic")
                    .SetHelp("topic to subscribe right color of stereo")
                    .Build();
    right_color_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&depth_topic_));
    auto flag = builder.SetLongName("--depth_topic")
                    .SetHelp("topic to subscribe depth of depth camera")
                    .Build();
    depth_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&lidar_topic_));
    auto flag = builder.SetLongName("--lidar_topic")
                    .SetHelp("topic to subscribe lidar")
                    .Build();
    lidar_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&map_topic_));
    auto flag = builder.SetLongName("--map_topic")
                    .SetHelp("topic to publish map")
                    .Build();
    map_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&pose_topic_));
    auto flag = builder.SetLongName("--pose_topic")
                    .SetHelp("topic to publish pose")
                    .Build();
    pose_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    IntDefaultFlag::Builder builder(MakeValueStore(&fps_, 10));
    auto flag = builder.SetLongName("--fps")
                    .SetHelp("Fps to run slam (default: 10)")
                    .Build();
    fps_flag_ = std::make_unique<IntDefaultFlag>(flag);
  }
}

SlamNodeCreateFlag::~SlamNodeCreateFlag() = default;

bool SlamNodeCreateFlag::Parse(FlagParser& parser) {
  bool parsed = PARSE_OPTIONAL_FLAG(parser, name_flag_, left_color_topic_flag_,
                                    right_color_topic_flag_, depth_topic_flag_,
                                    lidar_topic_flag_, map_topic_flag_,
                                    pose_topic_flag_, fps_flag_);

  if (parsed) return true;

  switch (current_slam_kind_) {
    case SLAM_KIND_NONE:
      if (slam_kind_flag_->Parse(parser)) {
        if (slam_kind_ == kHectorSlam) {
          current_slam_kind_ = SLAM_KIND_HECTOR_SLAM;
        }
        if (slam_kind_ == kOrb2Slam) {
          current_slam_kind_ = SLAM_KIND_ORB_SLAM2;
        }
        return true;
      }
    case SLAM_KIND_ORB_SLAM2:
      return orb_slam2_delegate_.Parse(parser);
    case SLAM_KIND_HECTOR_SLAM:
      return hector_slam_delegate_.Parse(parser);
  }

  return false;
}

bool SlamNodeCreateFlag::Validate() const {
  if (!CheckIfFlagPositive(fps_flag_)) return false;

  switch (current_slam_kind_) {
    case SLAM_KIND_NONE:
      std::cerr << kRedError << "slam_kind should be set." << std::endl;
      return false;
    case SLAM_KIND_ORB_SLAM2:
      return (CheckIfLeftColorTopicWasSet() ||
              CheckIfLeftAndRightColorTopicWasSet() ||
              CheckIfLeftColorAndDepthTopicWasSet()) &&
             orb_slam2_delegate_.Validate();
    case SLAM_KIND_HECTOR_SLAM:
      return CheckIfLidarTopicWasSet() && hector_slam_delegate_.Validate();
  }

  return true;
}

std::vector<std::string> SlamNodeCreateFlag::CollectUsages() const {
  switch (current_slam_kind_) {
    case SLAM_KIND_NONE: {
      std::vector<std::string> usages;
      usages.push_back("[--help]");
      AddUsage(usages, slam_kind_flag_, name_flag_, left_color_topic_flag_,
               right_color_topic_flag_, depth_topic_flag_, lidar_topic_flag_,
               map_topic_flag_, pose_topic_flag_, fps_flag_);
      return usages;
    }
    case SLAM_KIND_ORB_SLAM2:
      return orb_slam2_delegate_.CollectUsages();
    case SLAM_KIND_HECTOR_SLAM:
      return hector_slam_delegate_.CollectUsages();
  }
}

std::string SlamNodeCreateFlag::Description() const {
  switch (current_slam_kind_) {
    case SLAM_KIND_NONE:
      return "Manage Slam";
    case SLAM_KIND_ORB_SLAM2:
      return orb_slam2_delegate_.Description();
    case SLAM_KIND_HECTOR_SLAM:
      return hector_slam_delegate_.Description();
  }
}

std::vector<NamedHelpType> SlamNodeCreateFlag::CollectNamedHelps() const {
  switch (current_slam_kind_) {
    case SLAM_KIND_NONE: {
      return {
          std::make_pair(
              TextStyle::Blue("Slams: "),
              std::vector<std::string>{
                  MakeNamedHelpText(kHectorSlam,
                                    hector_slam_delegate_.Description()),
                  MakeNamedHelpText(kOrb2Slam,
                                    orb_slam2_delegate_.Description()),
              }),
          std::make_pair(
              kYellowOptions,
              std::vector<std::string>{
                  slam_kind_flag_->help(), name_flag_->help(),
                  left_color_topic_flag_->help(),
                  right_color_topic_flag_->help(), depth_topic_flag_->help(),
                  lidar_topic_flag_->help(), map_topic_flag_->help(),
                  pose_topic_flag_->help(), fps_flag_->help()}),
      };
    }
    case SLAM_KIND_ORB_SLAM2:
      return orb_slam2_delegate_.CollectNamedHelps();
    case SLAM_KIND_HECTOR_SLAM:
      return hector_slam_delegate_.CollectNamedHelps();
  }
}

bool SlamNodeCreateFlag::CheckIfLeftColorTopicWasSet(
    bool emit_error_message) const {
  if (!left_color_topic_flag_->is_set()) {
    if (emit_error_message)
      std::cerr << kRedError << "left_color_topic should be set." << std::endl;
    return false;
  }
  return true;
}

bool SlamNodeCreateFlag::CheckIfLeftAndRightColorTopicWasSet(
    bool emit_error_message) const {
  if (!(left_color_topic_flag_->is_set() &&
        right_color_topic_flag_->is_set())) {
    if (emit_error_message)
      std::cerr << kRedError
                << "left_color_topic and right_color_topic should be set."
                << std::endl;
    return false;
  }
  return true;
}

bool SlamNodeCreateFlag::CheckIfLeftColorAndDepthTopicWasSet(
    bool emit_error_message) const {
  if (!(left_color_topic_flag_->is_set() && depth_topic_flag_->is_set())) {
    if (emit_error_message)
      std::cerr << kRedError
                << "left_color_topic and depth_topic should be set."
                << std::endl;
    return false;
  }
  return true;
}

bool SlamNodeCreateFlag::CheckIfLidarTopicWasSet(
    bool emit_error_message) const {
  if (!lidar_topic_flag_->is_set()) {
    std::cerr << kRedError << "lidar_topic should be set." << std::endl;
    return false;
  }
  return true;
}

}  // namespace felicia