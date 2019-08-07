#ifndef FELICIA_EXAMPLES_SLAM_SLAM_NODE_CREATE_FLAG_H_
#define FELICIA_EXAMPLES_SLAM_SLAM_NODE_CREATE_FLAG_H_

#include "felicia/examples/slam/hector_slam/hector_slam_flag.h"
#include "felicia/examples/slam/orb_slam2/orb_slam2_flag.h"

namespace felicia {

class SlamNodeCreateFlag : public FlagParser::Delegate {
 public:
  enum SlamKind {
    SLAM_KIND_NONE,
    SLAM_KIND_HECTOR_SLAM,
    SLAM_KIND_ORB_SLAM2,
  };

  SlamNodeCreateFlag();
  ~SlamNodeCreateFlag();

  const StringFlag* name_flag() const { return name_flag_.get(); }
  const StringFlag* left_color_topic() const {
    return left_color_topic_flag_.get();
  }
  const StringFlag* right_color_topic() const {
    return right_color_topic_flag_.get();
  }
  const StringFlag* depth_topic() const { return depth_topic_flag_.get(); }
  const StringFlag* lidar_topic_flag() const { return lidar_topic_flag_.get(); }
  const StringFlag* map_topic_flag() const { return map_topic_flag_.get(); }
  const StringFlag* pose_topic_flag() const { return pose_topic_flag_.get(); }
  const IntDefaultFlag* fps_flag() const { return fps_flag_.get(); }

  const hector_slam::HectorSlamFlag& hector_slam_delegate() const {
    return hector_slam_delegate_;
  }

  const orb_slam2::OrbSlam2Flag& orb_slam2_delegate() const {
    return orb_slam2_delegate_;
  }

  SlamKind slam_kind() const { return current_slam_kind_; }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

  bool CheckIfLeftColorTopicWasSet(bool emit_error_message = false) const;
  bool CheckIfLeftAndRightColorTopicWasSet(
      bool emit_error_message = false) const;
  bool CheckIfLeftColorAndDepthTopicWasSet(
      bool emit_error_message = false) const;
  bool CheckIfLidarTopicWasSet(bool emit_error_message = false) const;

 private:
  std::string slam_kind_;
  std::string name_;
  std::string left_color_topic_;
  std::string right_color_topic_;
  std::string depth_topic_;
  std::string lidar_topic_;
  std::string map_topic_;
  std::string pose_topic_;
  int fps_;
  std::unique_ptr<StringChoicesFlag> slam_kind_flag_;
  std::unique_ptr<StringFlag> name_flag_;
  std::unique_ptr<StringFlag> left_color_topic_flag_;
  std::unique_ptr<StringFlag> right_color_topic_flag_;
  std::unique_ptr<StringFlag> depth_topic_flag_;
  std::unique_ptr<StringFlag> lidar_topic_flag_;
  std::unique_ptr<StringFlag> map_topic_flag_;
  std::unique_ptr<StringFlag> pose_topic_flag_;
  std::unique_ptr<IntDefaultFlag> fps_flag_;

  SlamKind current_slam_kind_;
  hector_slam::HectorSlamFlag hector_slam_delegate_;
  orb_slam2::OrbSlam2Flag orb_slam2_delegate_;

  DISALLOW_COPY_AND_ASSIGN(SlamNodeCreateFlag);
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_SLAM_NODE_CREATE_FLAG_H_