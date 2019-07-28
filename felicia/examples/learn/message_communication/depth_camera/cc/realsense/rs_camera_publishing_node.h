#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_REALSENSE_RS_CAMERA_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_REALSENSE_RS_CAMERA_PUBLISHING_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/vendors/realsense/rs_camera_factory.h"

namespace felicia {

class RsCameraPublishingNode : public NodeLifecycle {
 public:
  RsCameraPublishingNode(const std::string& color_topic,
                         const std::string& depth_topic,
                         const std::string& pointcloud_topic,
                         const std::string& imu_topic,
                         const CameraDescriptor& camera_descriptor)
      : color_topic_(color_topic),
        depth_topic_(depth_topic),
        pointcloud_topic_(pointcloud_topic),
        imu_topic_(imu_topic),
        camera_descriptor_(camera_descriptor),
        requested_color_format_(CameraFormat(640, 480, PIXEL_FORMAT_BGR, 30)),
        requested_depth_format_(CameraFormat(640, 480, PIXEL_FORMAT_Z16, 30)),
        requested_gyro_format_(ImuFormat(200)),
        requested_accel_format_(ImuFormat(63)),
        imu_filter_kind_(ImuFilterFactory::MADGWICK_FILTER_KIND) {
    CHECK(!(color_topic_.empty() && depth_topic_.empty() &&
            imu_topic_.empty() && pointcloud_topic_.empty()))
        << "At least one of topic should be not empty";
  }

  void OnInit() override {
    std::cout << "RsCameraPublishingNode::OnInit()" << std::endl;
    camera_ = RsCameraFactory::NewDepthCamera(camera_descriptor_);
    Status s = camera_->Init();
    CHECK(s.ok()) << s;
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "RsCameraPublishingNode::OnDidCreate()" << std::endl;
    node_info_ = node_info;
    RequestPublish();
  }

  void OnError(const Status& s) override {
    std::cout << "RsCameraPublishingNode::OnError()" << std::endl;
    LOG(ERROR) << s;
  }

  void RequestPublish() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.is_dynamic_buffer = true;
    settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

    if (!color_topic_.empty()) {
      color_publisher_.RequestPublish(
          node_info_, color_topic_,
          ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_WS, settings,
          ::base::BindOnce(&RsCameraPublishingNode::OnRequestPublish,
                           ::base::Unretained(this)));
    }

    if (!depth_topic_.empty()) {
      depth_publisher_.RequestPublish(
          node_info_, depth_topic_,
          ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_WS, settings,
          ::base::BindOnce(&RsCameraPublishingNode::OnRequestPublish,
                           ::base::Unretained(this)));
    }

    if (!pointcloud_topic_.empty()) {
      pointcloud_publisher_.RequestPublish(
          node_info_, pointcloud_topic_,
          ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_WS, settings,
          ::base::BindOnce(&RsCameraPublishingNode::OnRequestPublish,
                           ::base::Unretained(this)));
    }

    if (!imu_topic_.empty()) {
      imu_publisher_.RequestPublish(
          node_info_, imu_topic_,
          ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_WS, settings,
          ::base::BindOnce(&RsCameraPublishingNode::OnRequestPublish,
                           ::base::Unretained(this)));
    }
  }

  void OnRequestPublish(const Status& s) {
    std::cout << "RsCameraPublishingNode::OnRequestPublish()" << std::endl;
    if (s.ok()) {
      if (!color_topic_.empty() && !color_publisher_.IsRegistered()) return;
      if (!depth_topic_.empty() && !depth_publisher_.IsRegistered()) return;
      if (!pointcloud_topic_.empty() && !pointcloud_publisher_.IsRegistered())
        return;
      if (!imu_topic_.empty() && !imu_publisher_.IsRegistered()) return;

      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(
          FROM_HERE, ::base::BindOnce(&RsCameraPublishingNode::StartCamera,
                                      ::base::Unretained(this)));
    } else {
      LOG(ERROR) << s;
    }
  }

  void StartCamera() {
    if (camera_->IsStarted()) return;

    StatusOr<::rs2::sensor> status_or = camera_->sensor(RS_COLOR);
    if (status_or.ok()) {
      ::rs2::sensor& s = status_or.ValueOrDie();
      camera_->SetOption(s, RS2_OPTION_EMITTER_ENABLED, 1);
    }

    RsCamera::StartParams params;
    params.status_callback = ::base::BindRepeating(
        &RsCameraPublishingNode::OnCameraError, ::base::Unretained(this));

    params.requested_color_format = requested_color_format_;
    params.requested_depth_format = requested_depth_format_;
    if (!color_topic_.empty()) {
      params.requested_color_format = requested_color_format_;
      params.color_frame_callback = ::base::BindRepeating(
          &RsCameraPublishingNode::OnColorFrame, ::base::Unretained(this));
    }

    if (!depth_topic_.empty()) {
      params.requested_depth_format = requested_depth_format_;
      params.depth_frame_callback = ::base::BindRepeating(
          &RsCameraPublishingNode::OnDepthFrame, ::base::Unretained(this));
    }

    if (!pointcloud_topic_.empty()) {
      StatusOr<::rs2::sensor> status_or = camera_->sensor(RS_COLOR);
      if (status_or.ok()) {
        params.named_filters.push_back(RsCamera::NamedFilter(
            RsCamera::NamedFilter::POINTCLOUD,
            std::make_shared<::rs2::pointcloud>(RS_COLOR.stream_type,
                                                RS_COLOR.stream_index)));
      } else {
        status_or = camera_->sensor(RS_DEPTH);
        if (status_or.ok()) {
          params.named_filters.push_back(
              RsCamera::NamedFilter(RsCamera::NamedFilter::COLORIZER,
                                    std::make_shared<::rs2::colorizer>()));
          params.named_filters.push_back(RsCamera::NamedFilter(
              RsCamera::NamedFilter::POINTCLOUD,
              std::make_shared<::rs2::pointcloud>(RS_DEPTH.stream_type,
                                                  RS_DEPTH.stream_index)));
        }
      }
      params.pointcloud_frame_callback = ::base::BindRepeating(
          &RsCameraPublishingNode::OnPointcloudFrame, ::base::Unretained(this));
    }

    if (!imu_topic_.empty()) {
      params.requested_accel_format = requested_accel_format_;
      params.requested_gyro_format = requested_gyro_format_;
      params.imu_filter_kind = imu_filter_kind_;
      params.imu_frame_callback = ::base::BindRepeating(
          &RsCameraPublishingNode::OnImuFrame, ::base::Unretained(this));
    }

    Status s = camera_->Start(params);
    if (s.ok()) {
      if (!color_topic_.empty()) {
        std::cout << "Color Fomrat: " << camera_->color_format() << std::endl;
      }
      if (!depth_topic_.empty()) {
        std::cout << "Depth Format: " << camera_->depth_format() << std::endl;
      }
      if (!imu_topic_.empty()) {
        std::cout << "Accel Fomrat: " << camera_->accel_format() << std::endl;
        std::cout << "Gyro Format: " << camera_->gyro_format() << std::endl;
      }
      // MasterProxy& master_proxy = MasterProxy::GetInstance();
      // master_proxy.PostDelayedTask(
      //     FROM_HERE,
      //     ::base::BindOnce(&RsCameraPublishingNode::RequestUnpublish,
      //                      ::base::Unretained(this)),
      //     ::base::TimeDelta::FromSeconds(10));
    } else {
      LOG(ERROR) << s;
    }
  }

  void OnColorFrame(CameraFrame color_frame) {
    if (color_publisher_.IsUnregistered()) return;

    color_publisher_.Publish(
        color_frame.ToCameraFrameMessage(),
        ::base::BindRepeating(&RsCameraPublishingNode::OnPublishColor,
                              ::base::Unretained(this)));
  }

  void OnDepthFrame(DepthCameraFrame depth_frame) {
    if (depth_publisher_.IsUnregistered()) return;

    depth_publisher_.Publish(
        depth_frame.ToDepthCameraFrameMessage(),
        ::base::BindRepeating(&RsCameraPublishingNode::OnPublishDepth,
                              ::base::Unretained(this)));
  }

  void OnPointcloudFrame(PointcloudFrame pointcloud_frame) {
    if (pointcloud_publisher_.IsUnregistered()) return;

    pointcloud_publisher_.Publish(
        pointcloud_frame.ToPointcloudFrameMessage(),
        ::base::BindRepeating(&RsCameraPublishingNode::OnPublishPointcloud,
                              ::base::Unretained(this)));
  }

  void OnImuFrame(const ImuFrame& imu_frame) {
    if (imu_publisher_.IsUnregistered()) return;

    if (imu_frame.timestamp() - last_timestamp_ <
        ::base::TimeDelta::FromMilliseconds(100))
      return;

    imu_publisher_.Publish(
        imu_frame.ToImuFrameMessage(),
        ::base::BindRepeating(&RsCameraPublishingNode::OnPublishImu,
                              ::base::Unretained(this)));

    last_timestamp_ = imu_frame.timestamp();
  }

  void OnCameraError(const Status& s) { LOG(ERROR) << s; }

  void OnPublishColor(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

  void OnPublishDepth(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

  void OnPublishPointcloud(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

  void OnPublishImu(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

  void RequestUnpublish() {
    if (!color_topic_.empty()) {
      color_publisher_.RequestUnpublish(
          node_info_, color_topic_,
          ::base::BindOnce(&RsCameraPublishingNode::OnRequestUnpublish,
                           ::base::Unretained(this)));
    }

    if (!depth_topic_.empty()) {
      depth_publisher_.RequestUnpublish(
          node_info_, depth_topic_,
          ::base::BindOnce(&RsCameraPublishingNode::OnRequestUnpublish,
                           ::base::Unretained(this)));
    }

    if (!pointcloud_topic_.empty()) {
      pointcloud_publisher_.RequestUnpublish(
          node_info_, pointcloud_topic_,
          ::base::BindOnce(&RsCameraPublishingNode::OnRequestUnpublish,
                           ::base::Unretained(this)));
    }

    if (!imu_topic_.empty()) {
      imu_publisher_.RequestUnpublish(
          node_info_, imu_topic_,
          ::base::BindOnce(&RsCameraPublishingNode::OnRequestUnpublish,
                           ::base::Unretained(this)));
    }
  }

  void OnRequestUnpublish(const Status& s) {
    std::cout << "RsCameraPublishingNode::OnRequestUnpublish()" << std::endl;
    if (s.ok()) {
      if (!color_topic_.empty() && !color_publisher_.IsUnregistered()) return;
      if (!depth_topic_.empty() && !depth_publisher_.IsUnregistered()) return;
      if (!pointcloud_topic_.empty() && !pointcloud_publisher_.IsUnregistered())
        return;
      if (!imu_topic_.empty() && !imu_publisher_.IsUnregistered()) return;

      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(
          FROM_HERE, ::base::BindOnce(&RsCameraPublishingNode::StopCamera,
                                      ::base::Unretained(this)));
    } else {
      LOG(ERROR) << s;
    }
  }

  void StopCamera() {
    Status s = camera_->Stop();
    LOG_IF(ERROR, !s.ok()) << s;
  }

 private:
  NodeInfo node_info_;
  std::string color_topic_;
  std::string depth_topic_;
  std::string pointcloud_topic_;
  std::string imu_topic_;
  CameraDescriptor camera_descriptor_;
  CameraFormat requested_color_format_;
  CameraFormat requested_depth_format_;
  ImuFormat requested_gyro_format_;
  ImuFormat requested_accel_format_;
  ImuFilterFactory::ImuFilterKind imu_filter_kind_;
  Publisher<CameraFrameMessage> color_publisher_;
  Publisher<DepthCameraFrameMessage> depth_publisher_;
  Publisher<PointcloudFrameMessage> pointcloud_publisher_;
  Publisher<ImuFrameMessage> imu_publisher_;
  std::unique_ptr<RsCamera> camera_;
  ::base::TimeDelta last_timestamp_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_REALSENSE_RS_CAMERA_PUBLISHING_NODE_H_