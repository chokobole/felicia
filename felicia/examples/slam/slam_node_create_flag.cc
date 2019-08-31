#include "felicia/examples/slam/slam_node_create_flag.h"

#include "felicia/core/util/command_line_interface/text_constants.h"

namespace felicia {

static const char* kHectorSlam = "HectorSlam";
#if defined(HAS_ORB_SLAM2)
static const char* kOrb2Slam = "OrbSlam2";
#endif

SlamNodeCreateFlag::SlamNodeCreateFlag() : current_slam_kind_(SLAM_KIND_NONE) {
  {
    StringChoicesFlag::Builder builder(MakeValueStore(
        &slam_kind_, base::EmptyString(), Choices<std::string> {
          kHectorSlam,
#if defined(HAS_ORB_SLAM2)
              kOrb2Slam
#endif
        }));
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
    StringFlag::Builder builder(MakeValueStore(&frame_topic_));
    auto flag = builder.SetLongName("--frame_topic")
                    .SetHelp("topic to publish frame")
                    .Build();
    frame_topic_flag_ = std::make_unique<StringFlag>(flag);
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
    FloatDefaultFlag::Builder builder(MakeValueStore(&color_fps_, 30.f));
    auto flag = builder.SetLongName("--color_fps")
                    .SetHelp("color fps to run slam (default: 30)")
                    .Build();
    color_fps_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&depth_fps_, 30.f));
    auto flag = builder.SetLongName("--depth_fps")
                    .SetHelp("depth fps to run slam (default: 30)")
                    .Build();
    depth_fps_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&lidar_fps_, 5.5f));
    auto flag = builder.SetLongName("--lidar_fps")
                    .SetHelp("lidar fps to run slam (default: 5.5)")
                    .Build();
    lidar_fps_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
}

SlamNodeCreateFlag::~SlamNodeCreateFlag() = default;

bool SlamNodeCreateFlag::Parse(FlagParser& parser) {
  bool parsed = PARSE_OPTIONAL_FLAG(
      parser, name_flag_, left_color_topic_flag_, right_color_topic_flag_,
      depth_topic_flag_, lidar_topic_flag_, frame_topic_flag_, map_topic_flag_,
      pose_topic_flag_, color_fps_flag_, depth_fps_flag_, lidar_fps_flag_);

  if (parsed) return true;

  switch (current_slam_kind_) {
    case SLAM_KIND_NONE:
      if (slam_kind_flag_->Parse(parser)) {
        if (slam_kind_ == kHectorSlam) {
          current_slam_kind_ = SLAM_KIND_HECTOR_SLAM;
        }
#if defined(HAS_ORB_SLAM2)
        if (slam_kind_ == kOrb2Slam) {
          current_slam_kind_ = SLAM_KIND_ORB_SLAM2;
        }
#endif
        return true;
      }
#if defined(HAS_ORB_SLAM2)
    case SLAM_KIND_ORB_SLAM2:
      return orb_slam2_delegate_.Parse(parser);
#endif
    case SLAM_KIND_HECTOR_SLAM:
      return hector_slam_delegate_.Parse(parser);
  }

  return false;
}

bool SlamNodeCreateFlag::Validate() const {
  if (!CheckIfFlagPositive(color_fps_flag_)) return false;
  if (!CheckIfFlagPositive(depth_fps_flag_)) return false;
  if (!CheckIfFlagPositive(lidar_fps_flag_)) return false;

  switch (current_slam_kind_) {
    case SLAM_KIND_NONE:
      std::cerr << kRedError << "slam_kind should be set." << std::endl;
      return false;
#if defined(HAS_ORB_SLAM2)
    case SLAM_KIND_ORB_SLAM2: {
      return (CheckIfLeftColorTopicWasSet() ||
              CheckIfLeftAndRightColorTopicWasSet() ||
              CheckIfLeftColorAndDepthTopicWasSet()) &&
             orb_slam2_delegate_.Validate();
    }
#endif
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
               frame_topic_flag_, map_topic_flag_, pose_topic_flag_,
               color_fps_flag_, depth_fps_flag_, lidar_fps_flag_);
      return usages;
    }
#if defined(HAS_ORB_SLAM2)
    case SLAM_KIND_ORB_SLAM2:
      return orb_slam2_delegate_.CollectUsages();
#endif
    case SLAM_KIND_HECTOR_SLAM:
      return hector_slam_delegate_.CollectUsages();
  }
}

std::string SlamNodeCreateFlag::Description() const {
  switch (current_slam_kind_) {
    case SLAM_KIND_NONE:
      return "Manage Slam";
#if defined(HAS_ORB_SLAM2)
    case SLAM_KIND_ORB_SLAM2:
      return orb_slam2_delegate_.Description();
#endif
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
              std::vector<std::string> {
                MakeNamedHelpText(kHectorSlam,
                                  hector_slam_delegate_.Description()),
#if defined(HAS_ORB_SLAM2)
                    MakeNamedHelpText(kOrb2Slam,
                                      orb_slam2_delegate_.Description()),
#endif
              }),
          std::make_pair(
              kYellowOptions,
              std::vector<std::string>{
                  slam_kind_flag_->help(), name_flag_->help(),
                  left_color_topic_flag_->help(),
                  right_color_topic_flag_->help(), depth_topic_flag_->help(),
                  lidar_topic_flag_->help(), frame_topic_flag_->help(),
                  map_topic_flag_->help(), pose_topic_flag_->help(),
                  color_fps_flag_->help(), depth_fps_flag_->help(),
                  lidar_fps_flag_->help()}),
      };
    }
#if defined(HAS_ORB_SLAM2)
    case SLAM_KIND_ORB_SLAM2:
      return orb_slam2_delegate_.CollectNamedHelps();
#endif
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
    if (emit_error_message)
      std::cerr << kRedError << "lidar_topic should be set." << std::endl;
    return false;
  }
  return true;
}

}  // namespace felicia