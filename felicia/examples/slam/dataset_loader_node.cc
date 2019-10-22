#include "felicia/examples/slam/dataset_loader_node.h"

#include "felicia/core/lib/file/file_util.h"
#include "felicia/core/lib/image/image.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/core/util/command_line_interface/text_style.h"
#include "felicia/slam/dataset/euroc_dataset_loader.h"
#include "felicia/slam/dataset/kitti_dataset_loader.h"
#include "felicia/slam/dataset/tum_dataset_loader.h"

namespace felicia {

DatasetLoaderNode::DatasetLoaderNode(const DatasetFlag& dataset_flag)
    : dataset_flag_(dataset_flag),
      left_color_topic_(dataset_flag.left_color_topic_flag()->value()),
      right_color_topic_(dataset_flag.right_color_topic_flag()->value()),
      depth_topic_(dataset_flag.depth_topic_flag()->value()),
      lidar_topic_(dataset_flag.lidar_topic_flag()->value()),
      imu_topic_(dataset_flag.imu_topic_flag()->value()),
      pointcloud_topic_(dataset_flag.pointcloud_topic_flag()->value()),
      pose_topic_(dataset_flag.pose_topic_flag()->value()),
      color_fps_(dataset_flag.color_fps_flag()->value()),
      depth_fps_(dataset_flag.depth_fps_flag()->value()),
      lidar_fps_(dataset_flag.lidar_fps_flag()->value()),
      imu_fps_(dataset_flag.imu_fps_flag()->value()),
      pointcloud_fps_(dataset_flag.pointcloud_fps_flag()->value()),
      pose_fps_(dataset_flag.pose_fps_flag()->value()),
      topic_publish_count_(0) {}

void DatasetLoaderNode::OnInit() {
  base::FilePath path = ToFilePath(dataset_flag_.path_flag()->value());
  const int data_types = dataset_flag_.data_types();

  switch (dataset_flag_.dataset_kind()) {
    case DatasetFlag::DATASET_KIND_EUROC:
      delegate_.reset(new slam::EurocDatasetLoader(
          path, static_cast<slam::SensorData::DataType>(data_types)));
      break;
    case DatasetFlag::DATASET_KIND_KITTI:
      delegate_.reset(new slam::KittiDatasetLoader(
          path, static_cast<slam::SensorData::DataType>(data_types)));
      break;
    case DatasetFlag::DATASET_KIND_TUM:
      delegate_.reset(new slam::TumDatasetLoader(
          path, static_cast<slam::SensorData::DataType>(data_types)));
      break;
    case DatasetFlag::DATASET_KIND_NONE:
      NOTREACHED();
  }
  dataset_loader_.set_delegate(delegate_.get());
}

void DatasetLoaderNode::OnDidCreate(NodeInfo node_info) {
  node_info_ = std::move(node_info);
  RequestPublish();
}

void DatasetLoaderNode::OnRequestPublish(slam::SensorData::DataType data_type,
                                         Status s) {
  if (s.ok()) {
    LoadData(data_type);
  } else {
    LOG(ERROR) << s;
  }
}

void DatasetLoaderNode::RequestPublish() {
  communication::Settings settings;
  settings.queue_size = 1;
  settings.is_dynamic_buffer = true;
  settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

  ChannelDef::Type channel_type;
  ChannelDef::Type_Parse(dataset_flag_.channel_type_flag()->value(),
                         &channel_type);

  if (!left_color_topic_.empty()) {
    slam::SensorData::DataType data_type =
        slam::SensorData::DATA_TYPE_LEFT_CAMERA;
    if (dataset_flag_.left_as_gray_scale_flag()->value())
      data_type = slam::SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE;
    left_color_publisher_.RequestPublish(
        node_info_, left_color_topic_,
        channel_type | ChannelDef::CHANNEL_TYPE_WS, settings,
        base::BindOnce(&DatasetLoaderNode::OnRequestPublish,
                       base::Unretained(this), data_type));
    ++topic_publish_count_;
  }

  if (!right_color_topic_.empty()) {
    slam::SensorData::DataType data_type =
        slam::SensorData::DATA_TYPE_RIGHT_CAMERA;
    if (dataset_flag_.right_as_gray_scale_flag()->value())
      data_type = slam::SensorData::DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE;
    right_color_publisher_.RequestPublish(
        node_info_, right_color_topic_,
        channel_type | ChannelDef::CHANNEL_TYPE_WS, settings,
        base::BindOnce(&DatasetLoaderNode::OnRequestPublish,
                       base::Unretained(this), data_type));
    ++topic_publish_count_;
  }

  if (!depth_topic_.empty()) {
    depth_publisher_.RequestPublish(
        node_info_, depth_topic_, channel_type | ChannelDef::CHANNEL_TYPE_WS,
        settings,
        base::BindOnce(&DatasetLoaderNode::OnRequestPublish,
                       base::Unretained(this),
                       slam::SensorData::DATA_TYPE_DEPTH_CAMERA));
    ++topic_publish_count_;
  }

  if (!lidar_topic_.empty()) {
    lidar_publisher_.RequestPublish(
        node_info_, lidar_topic_, channel_type | ChannelDef::CHANNEL_TYPE_WS,
        settings,
        base::BindOnce(&DatasetLoaderNode::OnRequestPublish,
                       base::Unretained(this),
                       slam::SensorData::DATA_TYPE_LIDAR));
    ++topic_publish_count_;
  }

  if (!imu_topic_.empty()) {
    imu_publisher_.RequestPublish(
        node_info_, imu_topic_, channel_type | ChannelDef::CHANNEL_TYPE_WS,
        settings,
        base::BindOnce(&DatasetLoaderNode::OnRequestPublish,
                       base::Unretained(this),
                       slam::SensorData::DATA_TYPE_IMU));
    ++topic_publish_count_;
  }

  if (!pointcloud_topic_.empty()) {
    pointcloud_publisher_.RequestPublish(
        node_info_, pointcloud_topic_,
        channel_type | ChannelDef::CHANNEL_TYPE_WS, settings,
        base::BindOnce(&DatasetLoaderNode::OnRequestPublish,
                       base::Unretained(this),
                       slam::SensorData::DATA_TYPE_POINTCLOUD));
    ++topic_publish_count_;
  }

  if (!pose_topic_.empty()) {
    pose_publisher_.RequestPublish(
        node_info_, pose_topic_, channel_type | ChannelDef::CHANNEL_TYPE_WS,
        settings,
        base::BindOnce(&DatasetLoaderNode::OnRequestPublish,
                       base::Unretained(this),
                       slam::SensorData::DATA_TYPE_GROUND_TRUTH_POSE));
    ++topic_publish_count_;
  }
}

void DatasetLoaderNode::LoadData(slam::SensorData::DataType data_type) {
  StatusOr<slam::SensorData> status_or =
      dataset_loader_.Next(static_cast<int>(data_type));
  if (!status_or.ok()) {
    if (dataset_loader_.End(static_cast<int>(data_type))) {
      std::cout << TextStyle::Green(slam::SensorData::ToString(data_type))
                << " is end!" << std::endl;
    } else {
      LOG(ERROR) << status_or.status();
    }
    return;
  }
  double delay = 1;
  slam::SensorData sensor_data = std::move(status_or.ValueOrDie());
  if ((data_type == slam::SensorData::DATA_TYPE_LEFT_CAMERA ||
       data_type == slam::SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE) &&
      !left_color_topic_.empty()) {
    left_color_publisher_.Publish(
        std::move(sensor_data).left_camera_frame().ToCameraFrameMessage(false));
    delay = 1.0 / color_fps_;
  } else if ((data_type == slam::SensorData::DATA_TYPE_RIGHT_CAMERA ||
              data_type ==
                  slam::SensorData::DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE) &&
             !right_color_topic_.empty()) {
    right_color_publisher_.Publish(std::move(sensor_data)
                                       .right_camera_frame()
                                       .ToCameraFrameMessage(false));
    delay = 1.0 / color_fps_;
  } else if (data_type == slam::SensorData::DATA_TYPE_DEPTH_CAMERA &&
             !depth_topic_.empty()) {
    depth_publisher_.Publish(std::move(sensor_data)
                                 .depth_camera_frame()
                                 .ToDepthCameraFrameMessage(false));
    delay = 1.0 / depth_fps_;
  } else if (data_type == slam::SensorData::DATA_TYPE_IMU &&
             !imu_topic_.empty()) {
    bool publish = false;
    if (topic_publish_count_ == 1) {
      publish = true;
    } else {
      base::TimeDelta timestamp = timestamper_.timestamp();
      if (imu_fps_ > 30 && timestamp - last_imu_timestamp_ >
                               base::TimeDelta::FromSecondsD(1.0 / 30)) {
        publish = true;
        last_imu_timestamp_ = timestamp;
      }
    }

    if (publish)
      imu_publisher_.Publish(
          std::move(sensor_data).imu_frame().ToImuFrameMessage());
    delay = 1.0 / imu_fps_;
  } else if (data_type == slam::SensorData::DATA_TYPE_POINTCLOUD &&
             !pointcloud_topic_.empty()) {
    pointcloud_publisher_.Publish(
        std::move(sensor_data).pointcloud().ToPointcloudMessage(false));
    delay = 1.0 / pointcloud_fps_;
  } else if (data_type == slam::SensorData::DATA_TYPE_GROUND_TRUTH_POSE &&
             !pose_topic_.empty()) {
    bool publish = false;
    if (topic_publish_count_ == 1) {
      publish = true;
    } else {
      base::TimeDelta timestamp = timestamper_.timestamp();
      if (pose_fps_ > 30 && timestamp - last_pose_timestamp_ >
                                base::TimeDelta::FromSecondsD(1.0 / 30)) {
        publish = true;
        last_pose_timestamp_ = timestamp;
      }
    }

    if (publish)
      pose_publisher_.Publish(Pose3fToPose3fWithTimestampMessage(
          std::move(sensor_data).pose(), sensor_data.timestamp()));
    delay = 1.0 / pose_fps_;
  } else {
    return;
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostDelayedTask(
      FROM_HERE,
      base::BindOnce(&DatasetLoaderNode::LoadData, base::Unretained(this),
                     data_type),
      base::TimeDelta::FromSecondsD(delay));
}

}  // namespace felicia