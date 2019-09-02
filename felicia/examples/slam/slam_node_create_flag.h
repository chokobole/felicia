#ifndef FELICIA_EXAMPLES_SLAM_SLAM_NODE_CREATE_FLAG_H_
#define FELICIA_EXAMPLES_SLAM_SLAM_NODE_CREATE_FLAG_H_

#include "felicia/examples/slam/hector_slam_flag.h"
#if defined(HAS_ORB_SLAM2)
#include "felicia/examples/slam/orb_slam2_flag.h"
#endif

namespace felicia {

class SlamNodeCreateFlag : public FlagParser::Delegate {
 public:
  enum SlamKind {
    SLAM_KIND_NONE,
    SLAM_KIND_HECTOR_SLAM,
#if defined(HAS_ORB_SLAM2)
    SLAM_KIND_ORB_SLAM2,
#endif
  };

  SlamNodeCreateFlag();
  ~SlamNodeCreateFlag();

  const StringFlag* name_flag() const { return name_flag_.get(); }
  const StringFlag* left_color_topic_flag() const {
    return left_color_topic_flag_.get();
  }
  const StringFlag* right_color_topic_flag() const {
    return right_color_topic_flag_.get();
  }
  const StringFlag* depth_topic_flag() const { return depth_topic_flag_.get(); }
  const StringFlag* lidar_topic_flag() const { return lidar_topic_flag_.get(); }
  const StringFlag* frame_topic_flag() const { return frame_topic_flag_.get(); }
  const StringFlag* map_topic_flag() const { return map_topic_flag_.get(); }
  const StringFlag* pose_topic_flag() const { return pose_topic_flag_.get(); }
  const FloatDefaultFlag* color_fps_flag() const {
    return color_fps_flag_.get();
  }
  const FloatDefaultFlag* depth_fps_flag() const {
    return depth_fps_flag_.get();
  }
  const FloatDefaultFlag* lidar_fps_flag() const {
    return lidar_fps_flag_.get();
  }

  const HectorSlamFlag& hector_slam_delegate() const {
    return hector_slam_delegate_;
  }
#if defined(HAS_ORB_SLAM2)
  const OrbSlam2Flag& orb_slam2_delegate() const { return orb_slam2_delegate_; }
#endif

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
  std::string frame_topic_;
  std::string map_topic_;
  std::string pose_topic_;
  float color_fps_;
  float depth_fps_;
  float lidar_fps_;
  std::unique_ptr<StringChoicesFlag> slam_kind_flag_;
  std::unique_ptr<StringFlag> name_flag_;
  std::unique_ptr<StringFlag> left_color_topic_flag_;
  std::unique_ptr<StringFlag> right_color_topic_flag_;
  std::unique_ptr<StringFlag> depth_topic_flag_;
  std::unique_ptr<StringFlag> lidar_topic_flag_;
  std::unique_ptr<StringFlag> frame_topic_flag_;
  std::unique_ptr<StringFlag> map_topic_flag_;
  std::unique_ptr<StringFlag> pose_topic_flag_;
  std::unique_ptr<FloatDefaultFlag> color_fps_flag_;
  std::unique_ptr<FloatDefaultFlag> depth_fps_flag_;
  std::unique_ptr<FloatDefaultFlag> lidar_fps_flag_;

  SlamKind current_slam_kind_;
  HectorSlamFlag hector_slam_delegate_;
#if defined(HAS_ORB_SLAM2)
  OrbSlam2Flag orb_slam2_delegate_;
#endif

  DISALLOW_COPY_AND_ASSIGN(SlamNodeCreateFlag);
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_SLAM_NODE_CREATE_FLAG_H_