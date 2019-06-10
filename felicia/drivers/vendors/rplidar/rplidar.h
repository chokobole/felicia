#ifndef FELICIA_DRIVERS_VENDORS_RPLIDAR_H_
#define FELICIA_DRIVERS_VENDORS_RPLIDAR_H_

#include <rplidar.h>
#include "third_party/chromium/base/memory/weak_ptr.h"
#include "third_party/chromium/base/threading/thread.h"

#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/util/timestamp/timestamper.h"
#include "felicia/drivers/lidar/lidar_interface.h"

namespace felicia {

class RPlidar : public LidarInterface, public ::base::SupportsWeakPtr<RPlidar> {
 public:
  ~RPlidar();

  Status Init() override;
  Status Start(LidarFrameCallback lidar_frame_callback) override;
  Status Stop() override;

  Status GetSupportedScanModes(
      std::vector<rp::standalone::rplidar::RplidarScanMode>* scan_modes);
  Status Start(rp::standalone::rplidar::RplidarScanMode scan_mode,
               LidarFrameCallback lidar_frame_callback);

  void DoScanOnce();
  void DoScanLoop();

 private:
  friend class RPlidarFactory;

  RPlidar(const LidarEndpoint& lidar_endpoint);

  Status DoStart(rp::standalone::rplidar::RplidarScanMode* scan_mode,
                 LidarFrameCallback lidar_frame_callback);
  void DoScan();

  ::base::Thread thread_;
  Timestamper timestamper_;
  bool is_stopping_ = false;

  std::unique_ptr<rp::standalone::rplidar::RPlidarDriver> driver_;
  rp::standalone::rplidar::RplidarScanMode scan_mode_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(RPlidar);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_RPLIDAR_H_