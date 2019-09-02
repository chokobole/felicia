#include "felicia/examples/learn/message_communication/lidar/cc/rplidar/rplidar_publishing_node.h"

#include <csignal>

#include "felicia/drivers/vendors/rplidar/rplidar_factory.h"

namespace felicia {

namespace {

#if defined(OS_POSIX)
RPlidarPublishingNode* node = nullptr;

void Shutdown(int signal) {
  if (node) {
    node->StopLidar();
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.Stop();
}
#endif

}  // namespace

RPlidarPublishingNode::RPlidarPublishingNode(
    const RPLidarFlag& rplidar_flag,
    const drivers::LidarEndpoint& lidar_endpoint)
    : rplidar_flag_(rplidar_flag),
      topic_(rplidar_flag_.topic_flag()->value()),
      scan_mode_(rplidar_flag_.scan_mode_flag()->value()),
      lidar_endpoint_(lidar_endpoint) {}

void RPlidarPublishingNode::OnInit() {
  lidar_ = drivers::RPlidarFactory::NewLidar(lidar_endpoint_);
  Status s = lidar_->Init();
  CHECK(s.ok()) << s;
}

void RPlidarPublishingNode::OnDidCreate(const NodeInfo& node_info) {
#if defined(OS_POSIX)
  node = this;
#endif
  node_info_ = node_info;
  RequestPublish();
}

void RPlidarPublishingNode::OnRequestPublish(const Status& s) {
  if (s.ok()) {
    StartLidar();
  } else {
    LOG(ERROR) << s;
  }
}

void RPlidarPublishingNode::OnRequestUnpublish(const Status& s) {
  if (s.ok()) {
    StopLidar();
  } else {
    LOG(ERROR) << s;
  }
}

void RPlidarPublishingNode::RequestPublish() {
  communication::Settings settings;
  settings.queue_size = 1;
  settings.is_dynamic_buffer = true;
  settings.channel_settings.ws_settings.permessage_deflate_enabled = false;

  ChannelDef::Type channel_type;
  ChannelDef::Type_Parse(rplidar_flag_.channel_type_flag()->value(),
                         &channel_type);

  lidar_publisher_.RequestPublish(
      node_info_, topic_, channel_type | ChannelDef::CHANNEL_TYPE_WS, settings,
      base::BindOnce(&RPlidarPublishingNode::OnRequestPublish,
                     base::Unretained(this)));
}

void RPlidarPublishingNode::RequestUnpublish() {
  lidar_publisher_.RequestUnpublish(
      node_info_, topic_,
      base::BindOnce(&RPlidarPublishingNode::OnRequestUnpublish,
                     base::Unretained(this)));
}

void RPlidarPublishingNode::StartLidar() {
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
          s = lidar_->Start(scan_mode, base::BindRepeating(
                                           &RPlidarPublishingNode::OnLidarFrame,
                                           base::Unretained(this)));
          started = true;
          break;
        }
      }
    }
  }

  if (!started) {
    s = lidar_->Start(base::BindRepeating(&RPlidarPublishingNode::OnLidarFrame,
                                          base::Unretained(this)));
  }

  if (s.ok()) {
#if defined(OS_POSIX)
    // To handle general case when POSIX ask the process to quit.
    std::signal(SIGTERM, &felicia::Shutdown);
    // To handle Ctrl + C.
    std::signal(SIGINT, &felicia::Shutdown);
    // To handle when the terminal is closed.
    std::signal(SIGHUP, &felicia::Shutdown);
#endif
    lidar_->DoScanLoop();
    // MasterProxy& master_proxy = MasterProxy::GetInstance();
    // master_proxy.PostDelayedTask(
    //     FROM_HERE,
    //     base::BindOnce(&RPlidarPublishingNode::RequestUnpublish,
    //                      base::Unretained(this)),
    //     base::TimeDelta::FromSeconds(10));
  } else {
    LOG(ERROR) << s;
  }
}

void RPlidarPublishingNode::StopLidar() {
  Status s = lidar_->Stop();
  LOG_IF(ERROR, !s.ok()) << s;
}

void RPlidarPublishingNode::OnLidarFrame(drivers::LidarFrame&& lidar_frame) {
  if (lidar_publisher_.IsUnregistered()) return;

  lidar_publisher_.Publish(lidar_frame.ToLidarFrameMessage(false));
}

}  // namespace felicia