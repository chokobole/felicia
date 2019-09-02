#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_PUBLISHING_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/camera/camera_interface.h"
#include "felicia/examples/learn/message_communication/common/cc/camera_flag.h"

namespace felicia {

class CameraPublishingNode : public NodeLifecycle {
 public:
  CameraPublishingNode(const CameraFlag& camera_flag,
                       const drivers::CameraDescriptor& camera_descriptor);

  // NodeLifecycle methods
  void OnInit() override;
  void OnDidCreate(const NodeInfo& node_info) override;

 private:
  void OnRequestPublish(const Status& s);
  void OnRequestUnpublish(const Status& s);

  void RequestPublish();
  void RequestUnpublish();

  void StartCamera();
  void StopCamera();

  void OnCameraFrame(drivers::CameraFrame&& camera_frame);
  void OnCameraError(const Status& s);

  NodeInfo node_info_;
  const CameraFlag& camera_flag_;
  const std::string topic_;
  drivers::CameraDescriptor camera_descriptor_;
  Publisher<drivers::CameraFrameMessage> publisher_;
  std::unique_ptr<drivers::CameraInterface> camera_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_PUBLISHING_NODE_H_