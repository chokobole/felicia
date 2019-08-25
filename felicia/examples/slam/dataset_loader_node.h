#ifndef FELICIA_EXAMPLES_SLAM_DATASET_LOADER_NODE_H_
#define FELICIA_EXAMPLES_SLAM_DATASET_LOADER_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/lib/file/file_util.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/camera/camera_frame.h"
#include "felicia/drivers/camera/depth_camera_frame.h"
#include "felicia/drivers/lidar/lidar_frame.h"
#include "felicia/examples/slam/dataset_flag.h"
#include "felicia/slam/dataset/euroc_dataset_loader.h"
#include "felicia/slam/dataset/kitti_dataset_loader.h"
#include "felicia/slam/dataset/tum_dataset_loader.h"

namespace felicia {

class DatasetLoaderNode : public NodeLifecycle {
 public:
  typedef DatasetLoader<slam::SensorMetaData, slam::SensorData>
      DatasetLoaderType;

  DatasetLoaderNode(const DatasetFlag& dataset_flag)
      : dataset_flag_(dataset_flag),
        left_color_topic_(dataset_flag.left_color_topic_flag()->value()),
        right_color_topic_(dataset_flag.left_color_topic_flag()->value()),
        depth_topic_(dataset_flag.depth_topic_flag()->value()),
        lidar_topic_(dataset_flag.lidar_topic_flag()->value()),
        color_fps_(dataset_flag.color_fps_flag()->value()),
        depth_fps_(dataset_flag.depth_fps_flag()->value()),
        lidar_fps_(dataset_flag.lidar_fps_flag()->value()) {}

  void OnInit() override {
    base::FilePath path = ToFilePath(dataset_flag_.path_flag()->value());
    const int data_kind = dataset_flag_.data_kind_flag()->value();

    switch (dataset_flag_.dataset_kind()) {
      case DatasetFlag::DATASET_KIND_EUROC:
        delegate_.reset(new slam::EurocDatasetLoader(
            path, static_cast<slam::EurocDatasetLoader::DataKind>(data_kind)));
        break;
      case DatasetFlag::DATASET_KIND_KITTI:
        delegate_.reset(new slam::KittiDatasetLoader(
            path, static_cast<slam::KittiDatasetLoader::DataKind>(data_kind)));
        break;
      case DatasetFlag::DATASET_KIND_TUM:
        delegate_.reset(new slam::TumDatasetLoader(
            path, static_cast<slam::TumDatasetLoader::DataKind>(data_kind)));
        break;
      case DatasetFlag::DATASET_KIND_NONE:
        NOTREACHED();
    }
    dataset_loader_.set_delegate(delegate_.get());
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    node_info_ = node_info;
    RequestPublish();
  }

  void OnError(const Status& s) override { LOG(ERROR) << s; }

  void RequestPublish() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.is_dynamic_buffer = true;
    settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

    ChannelDef::Type channel_type;
    ChannelDef::Type_Parse(dataset_flag_.channel_type_flag()->value(),
                           &channel_type);

    if (!left_color_topic_.empty())
      left_color_publisher_.RequestPublish(
          node_info_, left_color_topic_, channel_type, settings,
          base::BindOnce(&DatasetLoaderNode::OnRequestPublish,
                         base::Unretained(this)));

    if (!right_color_topic_.empty())
      right_color_publisher_.RequestPublish(
          node_info_, right_color_topic_, channel_type, settings,
          base::BindOnce(&DatasetLoaderNode::OnRequestPublish,
                         base::Unretained(this)));

    if (!depth_topic_.empty())
      depth_publisher_.RequestPublish(
          node_info_, depth_topic_, channel_type, settings,
          base::BindOnce(&DatasetLoaderNode::OnRequestPublish,
                         base::Unretained(this)));

    if (!lidar_topic_.empty())
      lidar_publisher_.RequestPublish(
          node_info_, lidar_topic_, channel_type, settings,
          base::BindOnce(&DatasetLoaderNode::OnRequestPublish,
                         base::Unretained(this)));
  }

  void OnRequestPublish(const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s;
    if (!left_color_topic_.empty() && !left_color_publisher_.IsRegistered())
      return;
    if (!right_color_topic_.empty() && !right_color_publisher_.IsRegistered())
      return;
    if (!depth_topic_.empty() && !depth_publisher_.IsRegistered()) return;
    if (!lidar_topic_.empty() && !lidar_publisher_.IsRegistered()) return;
    StartLoad();
  }

  void StartLoad() {}

 private:
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