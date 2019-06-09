#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_RPLIDAR_RPLIDAR_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_RPLIDAR_RPLIDAR_PUBLISHING_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/vendors/rplidar/rplidar_factory.h"

namespace felicia {

class RPlidarPublishingNode : public NodeLifecycle {
 public:
  RPlidarPublishingNode(const std::string& topic,
                        const LidarEndpoint& lidar_endpoint)
      : topic_(topic), lidar_endpoint_(lidar_endpoint) {}

  void OnInit() override {
    std::cout << "RPlidarPublishingNode::OnInit()" << std::endl;
    lidar_ = RPlidarFactory::NewLidar(lidar_endpoint_);
    Status s = lidar_->Init();
    CHECK(s.ok()) << s;
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "RPlidarPublishingNode::OnDidCreate()" << std::endl;
    node_info_ = node_info;
    RequestPublish();
  }

  void OnError(const Status& s) override {
    std::cout << "RPlidarPublishingNode::OnError()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void RequestPublish() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.is_dynamic_buffer = true;
    settings.channel_settings.ws_settings.permessage_deflate_enabled = false;
  }

  void OnRequestPublish(const Status& s) {
    std::cout << "RPlidarPublishingNode::OnRequestPublish()" << std::endl;
    if (s.ok()) {
      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(FROM_HERE,
                            ::base::BindOnce(&RPlidarPublishingNode::StartLidar,
                                             ::base::Unretained(this)));
    } else {
      LOG(ERROR) << s.error_message();
    }
  }

  void StartLidar() {
    if (lidar_->IsStarted()) return;

    Status s = lidar_->Start();

    if (s.ok()) {
      // MasterProxy& master_proxy = MasterProxy::GetInstance();
      // master_proxy.PostDelayedTask(
      //     FROM_HERE,
      //     ::base::BindOnce(&RPlidarPublishingNode::RequestUnpublish,
      //                      ::base::Unretained(this)),
      //     ::base::TimeDelta::FromSeconds(10));
    } else {
      LOG(ERROR) << s.error_message();
    }
  }

  void RequestUnpublish() {}

  void OnRequestUnpublish(const Status& s) {
    std::cout << "RPlidarPublishingNode::OnRequestUnpublish()" << std::endl;
    if (s.ok()) {
      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(FROM_HERE,
                            ::base::BindOnce(&RPlidarPublishingNode::StopLidar,
                                             ::base::Unretained(this)));
    } else {
      LOG(ERROR) << s.error_message();
    }
  }

  void StopLidar() {
    Status s = lidar_->Stop();
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

 private:
  NodeInfo node_info_;
  std::string topic_;
  LidarEndpoint lidar_endpoint_;
  std::unique_ptr<RPlidar> lidar_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_RPLIDAR_RPLIDAR_PUBLISHING_NODE_H_