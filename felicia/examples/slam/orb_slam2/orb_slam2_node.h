#ifndef FELICIA_EXAMPLES_SLAM_ORB_SLAM2_ORB_SLAM2_NODE_H_
#define FELICIA_EXAMPLES_SLAM_ORB_SLAM2_ORB_SLAM2_NODE_H_

#include "third_party/chromium/base/strings/strcat.h"

#include "felicia/core/communication/publisher.h"
#include "felicia/core/communication/subscriber.h"
#include "felicia/core/lib/unit/geometry/pose.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/camera/camera_frame_message.pb.h"
#include "felicia/drivers/pointcloud/pointcloud_frame_message.pb.h"
#include "felicia/examples/slam/orb_slam2_flag.h"
#include "felicia/examples/slam/slam_node_create_flag.h"

#include "System.h"

namespace felicia {
namespace orb_slam2 {

class OrbSlam2Node : public NodeLifecycle {
 public:
  explicit OrbSlam2Node(const SlamNodeCreateFlag& slam_node_create_flag);

  // NodeLifecycle methods
  void OnInit() override;
  void OnDidCreate(const NodeInfo& node_info) override;

 private:
  void OnLeftColorMessage(drivers::CameraFrameMessage&& message);

  void RequestSubscribe();
  void RequestPublish();

  void OnPoseUpdated(const Posef& pose, base::TimeDelta timestamp);

  void Track(cv::Mat left_image, cv::Mat right_image, double timestamp);

  NodeInfo node_info_;
  const SlamNodeCreateFlag& slam_node_create_flag_;
  const std::string left_color_topic_;
  const std::string right_color_topic_;
  const std::string depth_topic_;
  const std::string frame_topic_;
  const std::string map_topic_;
  const std::string pose_topic_;
  const float color_fps_;
  const float depth_fps_;
  std::unique_ptr<ORB_SLAM2::System> orb_slam2_;
  Subscriber<drivers::CameraFrameMessage> left_color_subscriber_;
  Publisher<drivers::CameraFrameMessage> frame_publisher_;
  Publisher<PosefWithTimestampMessage> pose_publisher_;
  Publisher<drivers::PointcloudFrameMessage> map_publisher_;
};

}  // namespace orb_slam2
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_ORB_SLAM2_ORB_SLAM2_NODE_H_