#ifndef FELICIA_EXAMPLES_LEARN_TOPIC_LIDAR_CC_RPLIDAR_RPLIDAR_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_TOPIC_LIDAR_CC_RPLIDAR_RPLIDAR_PUBLISHING_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/vendors/rplidar/rplidar.h"
#include "felicia/examples/learn/topic/lidar/cc/rplidar/rplidar_flag.h"

namespace felicia {

class RPlidarPublishingNode : public NodeLifecycle {
 public:
  RPlidarPublishingNode(const RPLidarFlag& rplidar_flag,
                        const drivers::LidarEndpoint& lidar_endpoint);

  // NodeLifecycle methods
  void OnInit() override;
  void OnDidCreate(NodeInfo node_info) override;

 private:
  void OnRequestPublish(Status s);
  void OnRequestUnpublish(Status s);

  void StartLidar();
  void StopLidar();

  void RequestPublish();
  void RequestUnpublish();

  void OnLidarFrame(drivers::LidarFrame&& lidar_frame);

  NodeInfo node_info_;
  const RPLidarFlag& rplidar_flag_;
  const std::string topic_;
  const std::string scan_mode_;
  drivers::LidarEndpoint lidar_endpoint_;
  std::unique_ptr<drivers::RPlidar> lidar_;
  Publisher<drivers::LidarFrameMessage> lidar_publisher_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_TOPIC_LIDAR_CC_RPLIDAR_RPLIDAR_PUBLISHING_NODE_H_