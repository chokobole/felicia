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
                         const std::string& imu_topic,
                         const CameraDescriptor& camera_descriptor,
                         bool synched)
      : color_topic_(color_topic),
        depth_topic_(depth_topic),
        imu_topic_(imu_topic),
        camera_descriptor_(camera_descriptor),
        synched_(synched) {}

  void OnInit() override {
    std::cout << "RsCameraPublishingNode::OnInit()" << std::endl;
    camera_ = RsCameraFactory::NewDepthCamera(camera_descriptor_);
    CHECK(camera_->Init().ok());
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "RsCameraPublishingNode::OnDidCreate()" << std::endl;
    node_info_ = node_info;
    RequestPublish();
  }

  void OnError(const Status& s) override {
    std::cout << "RsCameraPublishingNode::OnError()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void RequestPublish() {
    ChannelDef channel_def;

    communication::Settings settings;
    settings.is_dynamic_buffer = true;

    color_publisher_.RequestPublish(
        node_info_, color_topic_, channel_def, settings,
        ::base::BindOnce(&RsCameraPublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));

    depth_publisher_.RequestPublish(
        node_info_, depth_topic_, channel_def, settings,
        ::base::BindOnce(&RsCameraPublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));

    imu_publisher_.RequestPublish(
        node_info_, imu_topic_, channel_def, settings,
        ::base::BindOnce(&RsCameraPublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));
  }

  void OnRequestPublish(const Status& s) {
    std::cout << "RsCameraPublishingNode::OnRequestPublish()" << std::endl;
    if (s.ok()) {
      if (!(color_publisher_.IsRegistered() && depth_publisher_.IsRegistered()))
        return;

      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(
          FROM_HERE, ::base::BindOnce(&RsCameraPublishingNode::StartCamera,
                                      ::base::Unretained(this)));
    } else {
      LOG(ERROR) << s.error_message();
    }
  }

  void StartCamera() {
    Status s;
    if (synched_) {
      s = camera_->Start(
          CameraFormat(640, 480, PIXEL_FORMAT_YUY2,
                       5), /* requested camera format */
          CameraFormat(640, 480, PIXEL_FORMAT_Z16,
                       5), /* requested depth format */
          ImuFormat(200),  /* requested gyro format */
          ImuFormat(63),   /* requested accel format */
          ImuFilterFactory::MadgwickFilterKind,
          ::base::BindRepeating(&RsCameraPublishingNode::OnDepthCameraFrame,
                                ::base::Unretained(this)),
          ::base::BindRepeating(&RsCameraPublishingNode::OnImu,
                                ::base::Unretained(this)),
          ::base::BindRepeating(&RsCameraPublishingNode::OnCameraError,
                                ::base::Unretained(this)));
    } else {
      s = camera_->Start(
          CameraFormat(640, 480, PIXEL_FORMAT_YUY2,
                       5), /* requested camera format */
          CameraFormat(640, 480, PIXEL_FORMAT_Z16,
                       5), /* requested depth format */
          ImuFormat(200),  /* requested gyro format */
          ImuFormat(63),   /* requested accel format */
          ImuFilterFactory::MadgwickFilterKind,
          ::base::BindRepeating(&RsCameraPublishingNode::OnColorFrame,
                                ::base::Unretained(this)),
          ::base::BindRepeating(&RsCameraPublishingNode::OnDepthFrame,
                                ::base::Unretained(this)),
          ::base::BindRepeating(&RsCameraPublishingNode::OnImu,
                                ::base::Unretained(this)),
          ::base::BindRepeating(&RsCameraPublishingNode::OnCameraError,
                                ::base::Unretained(this)));
    }
    if (s.ok()) {
      // MasterProxy& master_proxy = MasterProxy::GetInstance();
      // master_proxy.PostDelayedTask(
      //     FROM_HERE,
      //     ::base::BindOnce(&RsCameraPublishingNode::RequestUnpublish,
      //                      ::base::Unretained(this)),
      //     ::base::TimeDelta::FromSeconds(10));
    } else {
      LOG(ERROR) << s.error_message();
    }
  }

  void OnDepthCameraFrame(CameraFrame color_frame, CameraFrame depth_frame) {
    if (!color_publisher_.IsUnregistered()) {
      color_publisher_.Publish(
          color_frame.ToCameraFrameMessage(),
          ::base::BindOnce(&RsCameraPublishingNode::OnPublishColor,
                           ::base::Unretained(this)));
    }
    if (!depth_publisher_.IsUnregistered()) {
      depth_publisher_.Publish(
          depth_frame.ToCameraFrameMessage(),
          ::base::BindOnce(&RsCameraPublishingNode::OnPublishDepth,
                           ::base::Unretained(this)));
    }
  }

  void OnColorFrame(CameraFrame camera_frame) {
    if (color_publisher_.IsUnregistered()) return;

    color_publisher_.Publish(
        camera_frame.ToCameraFrameMessage(),
        ::base::BindOnce(&RsCameraPublishingNode::OnPublishColor,
                         ::base::Unretained(this)));
  }

  void OnDepthFrame(CameraFrame camera_frame) {
    if (depth_publisher_.IsUnregistered()) return;

    depth_publisher_.Publish(
        camera_frame.ToCameraFrameMessage(),
        ::base::BindOnce(&RsCameraPublishingNode::OnPublishDepth,
                         ::base::Unretained(this)));
  }

  void OnImu(const Imu& imu) {
    if (imu_publisher_.IsUnregistered()) return;

    if (imu.timestamp() - last_timestamp_ <
        ::base::TimeDelta::FromMilliseconds(100))
      return;

    imu_publisher_.Publish(
        imu.ToImuMessage(),
        ::base::BindOnce(&RsCameraPublishingNode::OnPublishImu,
                         ::base::Unretained(this)));

    last_timestamp_ = imu.timestamp();
  }

  void OnCameraError(const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void OnPublishColor(const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void OnPublishDepth(const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void OnPublishImu(const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void RequestUnpublish() {
    color_publisher_.RequestUnpublish(
        node_info_, color_topic_,
        ::base::BindOnce(&RsCameraPublishingNode::OnRequestUnpublish,
                         ::base::Unretained(this)));

    depth_publisher_.RequestUnpublish(
        node_info_, depth_topic_,
        ::base::BindOnce(&RsCameraPublishingNode::OnRequestUnpublish,
                         ::base::Unretained(this)));
  }

  void OnRequestUnpublish(const Status& s) {
    std::cout << "RsCameraPublishingNode::OnRequestUnpublish()" << std::endl;
    if (s.ok()) {
      if (!(color_publisher_.IsUnregistered() &&
            depth_publisher_.IsUnregistered() &&
            imu_publisher_.IsUnregistered()))
        return;

      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(
          FROM_HERE, ::base::BindOnce(&RsCameraPublishingNode::StopCamera,
                                      ::base::Unretained(this)));
    } else {
      LOG(ERROR) << s.error_message();
    }
  }

  void StopCamera() {
    Status s = camera_->Stop();
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

 private:
  NodeInfo node_info_;
  std::string color_topic_;
  std::string depth_topic_;
  std::string imu_topic_;
  CameraDescriptor camera_descriptor_;
  bool synched_;
  Publisher<CameraFrameMessage> color_publisher_;
  Publisher<CameraFrameMessage> depth_publisher_;
  Publisher<ImuMessage> imu_publisher_;
  std::unique_ptr<RsCamera> camera_;
  ::base::TimeDelta last_timestamp_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_REALSENSE_RS_CAMERA_PUBLISHING_NODE_H_