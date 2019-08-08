#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_RPLIDAR_RPLIDAR_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_RPLIDAR_RPLIDAR_PUBLISHING_NODE_H_

#include <csignal>

#include "felicia/core/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/vendors/rplidar/rplidar_factory.h"
#include "felicia/examples/learn/message_communication/lidar/cc/rplidar/rplidar_flag.h"

namespace felicia {

#ifdef OS_POSIX
class RPlidarPublishingNode;
RPlidarPublishingNode* node = nullptr;

void Shutdown(int signal);
#endif

class RPlidarPublishingNode : public NodeLifecycle {
 public:
  RPlidarPublishingNode(const RPLidarFlag& rplidar_flag,
                        const LidarEndpoint& lidar_endpoint)
      : rplidar_flag_(rplidar_flag),
        topic_(rplidar_flag_.topic_flag()->value()),
        scan_mode_(rplidar_flag_.scan_mode_flag()->value()),
        lidar_endpoint_(lidar_endpoint) {}

  void OnInit() override {
    std::cout << "RPlidarPublishingNode::OnInit()" << std::endl;
    lidar_ = RPlidarFactory::NewLidar(lidar_endpoint_);
    Status s = lidar_->Init();
    CHECK(s.ok()) << s;
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "RPlidarPublishingNode::OnDidCreate()" << std::endl;
#ifdef OS_POSIX
    node = this;
#endif
    node_info_ = node_info;
    RequestPublish();
  }

  void OnError(const Status& s) override {
    std::cout << "RPlidarPublishingNode::OnError()" << std::endl;
    LOG(ERROR) << s;
  }

  void RequestPublish() {
    communication::Settings settings;
    settings.queue_size = 1;
    settings.is_dynamic_buffer = true;
    settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

    ChannelDef::Type channel_type;
    ChannelDef::Type_Parse(rplidar_flag_.channel_type_flag()->value(),
                           &channel_type);

    lidar_publisher_.RequestPublish(
        node_info_, topic_, channel_type | ChannelDef::CHANNEL_TYPE_WS,
        settings,
        ::base::BindOnce(&RPlidarPublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));
  }

  void OnRequestPublish(const Status& s) {
    std::cout << "RPlidarPublishingNode::OnRequestPublish()" << std::endl;
    if (s.ok()) {
      StartLidar();
    } else {
      LOG(ERROR) << s;
    }
  }

  void StartLidar() {
    if (lidar_->IsStarted()) return;

    bool started = false;
    Status s;
    if (!scan_mode_.empty()) {
      std::vector<rp::standalone::rplidar::RplidarScanMode> scan_modes;
      s = lidar_->GetSupportedScanModes(&scan_modes);
      if (!s.ok()) {
        LOG(ERROR) << s;
      } else {
        for (auto& scan_mode : scan_modes) {
          if (scan_mode.scan_mode == scan_mode_) {
            s = lidar_->Start(
                scan_mode,
                ::base::BindRepeating(&RPlidarPublishingNode::OnLidarFrame,
                                      ::base::Unretained(this)));
            started = true;
            break;
          }
        }
      }
    }

    if (!started) {
      s = lidar_->Start(::base::BindRepeating(
          &RPlidarPublishingNode::OnLidarFrame, ::base::Unretained(this)));
    }

    if (s.ok()) {
#ifdef OS_POSIX
      // To handle general case when POSIX ask the process to quit.
      std::signal(SIGTERM, &::felicia::Shutdown);
      // To handle Ctrl + C.
      std::signal(SIGINT, &::felicia::Shutdown);
      // To handle when the terminal is closed.
      std::signal(SIGHUP, &::felicia::Shutdown);
#endif
      lidar_->DoScanLoop();
      // MasterProxy& master_proxy = MasterProxy::GetInstance();
      // master_proxy.PostDelayedTask(
      //     FROM_HERE,
      //     ::base::BindOnce(&RPlidarPublishingNode::RequestUnpublish,
      //                      ::base::Unretained(this)),
      //     ::base::TimeDelta::FromSeconds(10));
    } else {
      LOG(ERROR) << s;
    }
  }

  void OnLidarFrame(const LidarFrame& lidar_frame) {
    if (lidar_publisher_.IsUnregistered()) return;

    lidar_publisher_.Publish(
        lidar_frame.ToLidarFrameMessage(),
        ::base::BindRepeating(&RPlidarPublishingNode::OnPublishLidarFrame,
                              ::base::Unretained(this)));
  }

  void OnPublishLidarFrame(ChannelDef::Type type, const Status& s) {
    LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
  }

  void RequestUnpublish() {
    lidar_publisher_.RequestUnpublish(
        node_info_, topic_,
        ::base::BindOnce(&RPlidarPublishingNode::OnRequestUnpublish,
                         ::base::Unretained(this)));
  }

  void OnRequestUnpublish(const Status& s) {
    std::cout << "RPlidarPublishingNode::OnRequestUnpublish()" << std::endl;
    if (s.ok()) {
      StopLidar();
    } else {
      LOG(ERROR) << s;
    }
  }

  void StopLidar() {
    Status s = lidar_->Stop();
    LOG_IF(ERROR, !s.ok()) << s;
  }

 private:
  NodeInfo node_info_;
  const RPLidarFlag& rplidar_flag_;
  const std::string topic_;
  const std::string scan_mode_;
  LidarEndpoint lidar_endpoint_;
  std::unique_ptr<RPlidar> lidar_;
  Publisher<LidarFrameMessage> lidar_publisher_;
};

#ifdef OS_POSIX
void Shutdown(int signal) {
  if (node) {
    node->StopLidar();
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.Stop();
}
#endif

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_RPLIDAR_RPLIDAR_PUBLISHING_NODE_H_