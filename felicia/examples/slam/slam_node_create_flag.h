#ifndef FELICIA_EXAMPLES_SLAM_SLAM_NODE_CREATE_FLAG_H_
#define FELICIA_EXAMPLES_SLAM_SLAM_NODE_CREATE_FLAG_H_

#include "felicia/examples/slam/hecto_slam/hector_slam_flag.h"

namespace felicia {

class SlamNodeCreateFlag : public FlagParser::Delegate {
 public:
  SlamNodeCreateFlag();
  ~SlamNodeCreateFlag();

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  const StringFlag* name_flag() const { return name_flag_.get(); }
  const StringFlag* lidar_topic_flag() const { return lidar_topic_flag_.get(); }
  const StringFlag* map_topic_flag() const { return map_topic_flag_.get(); }
  const StringFlag* pose_topic_flag() const { return pose_topic_flag_.get(); }

  const HectorSlamFlag& hector_slam_flag() const { return hector_slam_flag_; }

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(
      name_flag_, lidar_topic_flag_, map_topic_flag_, pose_topic_flag_,
      hector_slam_flag_.map_resolution_flag_, hector_slam_flag_.map_size_flag_,
      hector_slam_flag_.map_start_x_flag_, hector_slam_flag_.map_start_y_flag_,
      hector_slam_flag_.map_multi_res_levels_flag_,
      hector_slam_flag_.map_update_distance_thresh_flag_,
      hector_slam_flag_.map_update_angle_thresh_flag_,
      hector_slam_flag_.update_factor_free_flag_,
      hector_slam_flag_.update_factor_occupied_flag_,
      hector_slam_flag_.laser_min_dist_flag_,
      hector_slam_flag_.laser_max_dist_flag_)

 private:
  std::string name_;
  std::string lidar_topic_;
  std::string map_topic_;
  std::string pose_topic_;
  std::unique_ptr<StringFlag> name_flag_;
  std::unique_ptr<StringFlag> lidar_topic_flag_;
  std::unique_ptr<StringFlag> map_topic_flag_;
  std::unique_ptr<StringFlag> pose_topic_flag_;

  HectorSlamFlag hector_slam_flag_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_SLAM_NODE_CREATE_FLAG_H_