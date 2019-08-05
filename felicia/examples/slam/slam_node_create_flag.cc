#include "felicia/examples/slam/slam_node_create_flag.h"

#include "felicia/core/util/command_line_interface/text_constants.h"

namespace felicia {

SlamNodeCreateFlag::SlamNodeCreateFlag() {
  {
    StringFlag::Builder builder(MakeValueStore(&name_));
    auto flag = builder.SetShortName("-n")
                    .SetLongName("--name")
                    .SetHelp("name for node")
                    .Build();
    name_flag_ = std::make_unique<StringFlag>(flag);
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
}

SlamNodeCreateFlag::~SlamNodeCreateFlag() = default;

bool SlamNodeCreateFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(
      parser, name_flag_, lidar_topic_flag_, map_topic_flag_, pose_topic_flag_,
      hector_slam_flag_.map_resolution_flag_, hector_slam_flag_.map_size_flag_,
      hector_slam_flag_.map_start_x_flag_, hector_slam_flag_.map_start_y_flag_,
      hector_slam_flag_.map_multi_res_levels_flag_,
      hector_slam_flag_.map_update_distance_thresh_flag_,
      hector_slam_flag_.map_update_angle_thresh_flag_,
      hector_slam_flag_.update_factor_free_flag_,
      hector_slam_flag_.update_factor_occupied_flag_,
      hector_slam_flag_.laser_min_dist_flag_,
      hector_slam_flag_.laser_max_dist_flag_);
}

bool SlamNodeCreateFlag::Validate() const {
  if (!lidar_topic_flag_->is_set()) {
    std::cerr << kRedError << "lidar_topic should be set." << std::endl;
    return false;
  }
  if (hector_slam_flag_.map_multi_res_levels_flag_ < 0) {
    std::cerr << kRedError << "level should be positive." << std::endl;
    return false;
  }

  return true;
}

}  // namespace felicia