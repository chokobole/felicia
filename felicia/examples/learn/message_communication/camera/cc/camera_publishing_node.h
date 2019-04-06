#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_PUBLISHING_NODE_H_

#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/camera/camera_factory.h"

namespace felicia {

class CameraPublishingNode : public NodeLifecycle {
 public:
  CameraPublishingNode(const std::string& topic,
                       const std::string& channel_type,
                       const std::string& display_name,
                       const std::string& device_id)
      : topic_(topic), display_name_(display_name), device_id_(device_id) {
    if (channel_type.compare("TCP") == 0) {
      channel_def_.set_type(ChannelDef::TCP);
    } else if (channel_type.compare("UDP") == 0) {
      channel_def_.set_type(ChannelDef::UDP);
    }
  }

  void OnInit() override {
    std::cout << "CameraPublishingNode::OnInit()" << std::endl;
    CameraDescriptor descriptor(display_name_, device_id_);
    std::unique_ptr<CameraInterface> camera =
        CameraFactory::NewCamera(descriptor);
    CHECK(camera->Init().ok());
    camera->Start(::base::BindRepeating(&CameraPublishingNode::OnImage,
                                        ::base::Unretained(this)));
  }

  void OnImage(StatusOr<CameraFrame> status_or) {
    LOG(INFO) << "CameraPublishingNode::OnImage" << std::endl;
    static int frame_number = 0;
    if (status_or.ok()) {
      CameraFrame frame = std::move(status_or.ValueOrDie());
      char filename[15];
      frame_number++;
      sprintf(filename, "frame-%d.argb", frame_number);
      FILE* fp = fopen(filename, "wb");

      fwrite(frame.data(), frame.size(), 1, fp);

      fflush(fp);
      fclose(fp);
    } else {
      LOG(ERROR) << status_or.status().error_message();
    }
  }

 private:
  NodeInfo node_info_;
  std::string topic_;
  ChannelDef channel_def_;
  std::string display_name_;
  std::string device_id_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_PUBLISHING_NODE_H_