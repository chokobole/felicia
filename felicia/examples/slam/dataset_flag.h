#ifndef FELICIA_EXAMPLES_SLAM_DATASET_FLAG_H_
#define FELICIA_EXAMPLES_SLAM_DATASET_FLAG_H_

#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

class DatasetFlag : public FlagParser::Delegate {
 public:
  enum DatasetKind {
    DATASET_KIND_NONE,
    DATASET_KIND_EUROC,
    DATASET_KIND_KITTI,
    DATASET_KIND_TUM,
  };

  DatasetFlag();
  ~DatasetFlag();
  const StringFlag* path_flag() const { return path_flag_.get(); }
  const IntFlag* data_kind_flag() const { return data_kind_flag_.get(); }
  const StringFlag* name_flag() const { return name_flag_.get(); }
  const StringChoicesFlag* channel_type_flag() const {
    return channel_type_flag_.get();
  }
  const StringFlag* left_color_topic_flag() const {
    return left_color_topic_flag_.get();
  }
  const StringFlag* right_color_topic_flag() const {
    return right_color_topic_flag_.get();
  }
  const StringFlag* depth_topic_flag() const { return depth_topic_flag_.get(); }
  const StringFlag* lidar_topic_flag() const { return lidar_topic_flag_.get(); }
  const FloatDefaultFlag* color_fps_flag() const {
    return color_fps_flag_.get();
  }
  const FloatDefaultFlag* depth_fps_flag() const {
    return depth_fps_flag_.get();
  }
  const FloatDefaultFlag* lidar_fps_flag() const {
    return lidar_fps_flag_.get();
  }

  DatasetKind dataset_kind() const { return current_dataset_kind_; }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(dataset_kind_flag_, path_flag_,
                                          data_kind_flag_, name_flag_,
                                          channel_type_flag_,
                                          left_color_topic_flag_,
                                          right_color_topic_flag_,
                                          depth_topic_flag_, lidar_topic_flag_)

 private:
  std::string dataset_kind_;
  std::string path_;
  int data_kind_;
  std::string name_;
  std::string channel_type_;
  std::string left_color_topic_;
  std::string right_color_topic_;
  std::string depth_topic_;
  std::string lidar_topic_;
  float color_fps_;
  float depth_fps_;
  float lidar_fps_;
  std::unique_ptr<StringChoicesFlag> dataset_kind_flag_;
  std::unique_ptr<StringFlag> path_flag_;
  std::unique_ptr<IntFlag> data_kind_flag_;
  std::unique_ptr<StringFlag> name_flag_;
  std::unique_ptr<StringChoicesFlag> channel_type_flag_;
  std::unique_ptr<StringFlag> left_color_topic_flag_;
  std::unique_ptr<StringFlag> right_color_topic_flag_;
  std::unique_ptr<StringFlag> depth_topic_flag_;
  std::unique_ptr<StringFlag> lidar_topic_flag_;
  std::unique_ptr<FloatDefaultFlag> color_fps_flag_;
  std::unique_ptr<FloatDefaultFlag> depth_fps_flag_;
  std::unique_ptr<FloatDefaultFlag> lidar_fps_flag_;
  DatasetKind current_dataset_kind_;

  DISALLOW_COPY_AND_ASSIGN(DatasetFlag);
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_DATASET_FLAG_H_