#ifndef FELICIA_EXAMPLES_SLAM_DATASET_LOADER_NODE_H_
#define FELICIA_EXAMPLES_SLAM_DATASET_LOADER_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/core/util/dataset/dataset_loader.h"
#include "felicia/drivers/camera/camera_frame_message.pb.h"
#include "felicia/drivers/camera/depth_camera_frame_message.pb.h"
#include "felicia/drivers/lidar/lidar_frame_message.pb.h"
#include "felicia/examples/slam/dataset_flag.h"
#include "felicia/slam/dataset/sensor_data.h"
#include "felicia/slam/dataset/sensor_meta_data.h"

namespace felicia {

class DatasetLoaderNode : public NodeLifecycle {
 public:
  typedef DatasetLoader<slam::SensorMetaData, slam::SensorData>
      DatasetLoaderType;

  explicit DatasetLoaderNode(const DatasetFlag& dataset_flag);

  // NodeLifecycle methods
  void OnInit() override;
  void OnDidCreate(const NodeInfo& node_info) override;

 private:
  void OnRequestPublish(slam::SensorData::DataType data_type, const Status& s);

  void RequestPublish();

  void LoadData(slam::SensorData::DataType data_type);

  NodeInfo node_info_;
  const DatasetFlag& dataset_flag_;
  const std::string left_color_topic_;
  const std::string right_color_topic_;
  const std::string depth_topic_;
  const std::string lidar_topic_;
  const float color_fps_;
  const float depth_fps_;
  const float lidar_fps_;
  DatasetLoaderType dataset_loader_;
  std::unique_ptr<DatasetLoaderType::Delegate> delegate_;
  Publisher<drivers::CameraFrameMessage> left_color_publisher_;
  Publisher<drivers::CameraFrameMessage> right_color_publisher_;
  Publisher<drivers::DepthCameraFrameMessage> depth_publisher_;
  Publisher<drivers::LidarFrameMessage> lidar_publisher_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_DATASET_LOADER_NODE_H_