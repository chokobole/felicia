#ifndef FELICIA_EXAMPLES_SLAM_ORB_SLAM2_ORB_SLAM2_NODE_H_
#define FELICIA_EXAMPLES_SLAM_ORB_SLAM2_ORB_SLAM2_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/communication/subscriber.h"
#include "felicia/core/lib/file/file_util.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/camera/camera_frame.h"
#include "felicia/examples/slam/orb_slam2/orb_slam2_flag.h"
#include "felicia/examples/slam/orb_slam2/system.h"

namespace felicia {
namespace orb_slam2 {

class OrbSlam2Node : public NodeLifecycle, public System::Client {
 public:
  OrbSlam2Node(const std::string& left_color_topic,
               const std::string& right_color_topic,
               const std::string& depth_topic, const std::string& map_topic,
               const std::string& pose_topic, int fps,
               const OrbSlam2Flag& orb_slam2_flag)
      : left_color_topic_(left_color_topic),
        right_color_topic_(right_color_topic),
        depth_topic_(depth_topic),
        map_topic_(map_topic),
        pose_topic_(pose_topic),
        fps_(fps),
        orb_slam2_flag_(orb_slam2_flag) {}

  void OnInit() override {
    orb_slam2::System::SensorType sensor_type;
    if (!left_color_topic_.empty() && !depth_topic_.empty()) {
      sensor_type = orb_slam2::System::SENSOR_TYPE_RGBD;
    } else if (!left_color_topic_.empty() && !right_color_topic_.empty()) {
      sensor_type = orb_slam2::System::SENSOR_TYPE_STEREO;
    } else if (!left_color_topic_.empty()) {
      sensor_type = orb_slam2::System::SENSOR_TYPE_MONOCULAR;
    } else {
      NOTREACHED() << "Can't set sensor_type";
    }
    const std::string& voc_path = orb_slam2_flag_.voc_path_flag()->value();
    const std::string& settings_path =
        orb_slam2_flag_.settings_path_flag()->value();

    orb_slam2_ = std::make_unique<orb_slam2::System>(
        this, ::base::FilePath{ToFilePathString(voc_path)},
        ::base::FilePath{ToFilePathString(settings_path)}, sensor_type);
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    node_info_ = node_info;
    RequestSubscribe();
    RequestPublish();
  }

  void OnError(const Status& s) override { LOG(ERROR) << s; }

  void RequestSubscribe() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.period = ::base::TimeDelta::FromSeconds(1.0 / fps_);
    settings.is_dynamic_buffer = true;

    if (!left_color_topic_.empty()) {
      left_color_subscriber_.RequestSubscribe(
          node_info_, left_color_topic_,
          ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_SHM,
          ::base::BindRepeating(&OrbSlam2Node::OnLeftColorMessage,
                                ::base::Unretained(this)),
          ::base::BindRepeating(&OrbSlam2Node::OnSubscriptionError,
                                ::base::Unretained(this)),
          settings,
          ::base::BindOnce(&OrbSlam2Node::OnRequestSubscribe,
                           ::base::Unretained(this)));
    }
  }

  void RequestPublish() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.is_dynamic_buffer = true;
    settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

    if (!pose_topic_.empty()) {
      pose_publisher_.RequestPublish(
          node_info_, pose_topic_,
          ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_SHM |
              ChannelDef::CHANNEL_TYPE_WS,
          settings,
          ::base::BindOnce(&OrbSlam2Node::OnRequestPublish,
                           ::base::Unretained(this)));
    }
  }

  void OnLeftColorMessage(CameraFrameMessage&& message) {
    CameraFrame camera_frame = CameraFrame::FromCameraFrameMessage(message);
    orb_slam2_->TrackMonocular(std::move(camera_frame));
  }

  void OnSubscriptionError(const Status& s) { LOG(ERROR) << s; }

  void OnRequestSubscribe(const Status& s) { LOG_IF(ERROR, !s.ok()) << s; }

  void OnRequestPublish(const Status& s) { LOG_IF(ERROR, !s.ok()) << s; }

  void OnPoseUpdated(const Posef& pose, ::base::TimeDelta timestamp) override {
    if (pose_topic_.empty()) return;
    if (!pose_publisher_.IsRegistered()) return;
    pose_publisher_.Publish(PosefToPosefWithTimestampMessage(pose, timestamp),
                            ::base::BindRepeating(&OrbSlam2Node::OnPublishPose,
                                                  ::base::Unretained(this)));
  }

  void OnPublishPose(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

 private:
  NodeInfo node_info_;
  const std::string left_color_topic_;
  const std::string right_color_topic_;
  const std::string depth_topic_;
  const std::string map_topic_;
  const std::string pose_topic_;
  const int fps_;
  const OrbSlam2Flag& orb_slam2_flag_;
  std::unique_ptr<orb_slam2::System> orb_slam2_;
  Subscriber<CameraFrameMessage> left_color_subscriber_;
  Publisher<PosefWithTimestampMessage> pose_publisher_;
};

}  // namespace orb_slam2
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_ORB_SLAM2_ORB_SLAM2_NODE_H_