#include "felicia/drivers/vendors/rplidar/rplidar.h"

#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/strings/strcat.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

using namespace rp::standalone::rplidar;

namespace felicia {

RPlidar::RPlidar(const LidarEndpoint& lidar_endpoint)
    : LidarInterface(lidar_endpoint) {}

RPlidar::~RPlidar() = default;

Status RPlidar::Init() {
  if (!lidar_state_.IsStopped()) {
    return lidar_state_.InvalidStateError();
  }

  std::unique_ptr<RPlidarDriver> driver;
  if (lidar_endpoint_.type() == LidarEndpoint::TCP) {
    driver = ::base::WrapUnique(RPlidarDriver::CreateDriver(DRIVER_TYPE_TCP));
  } else {
    driver =
        ::base::WrapUnique(RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT));
  }

  if (!driver) {
    return errors::Unavailable("Failed to create driver");
  }

  u_result result;
  if (lidar_endpoint_.type() == LidarEndpoint::TCP) {
    result = driver->connect(lidar_endpoint_.ip().c_str(),
                             lidar_endpoint_.tcp_port());
  } else {
    result = driver->connect(lidar_endpoint_.serial_port().c_str(),
                             lidar_endpoint_.buadrate());
  }

  if (IS_FAIL(result)) {
    return errors::Unavailable(::base::StrCat(
        {"Failed to connect to the endpoint", lidar_endpoint_.ToString()}));
  }

  rplidar_response_device_info_t devinfo;
  result = driver->getDeviceInfo(devinfo);
  if (IS_OK(result)) {
    std::string text;
    ::base::StringAppendF(&text, "%s", "RPLIDAR S/N: ");
    for (int i = 0; i < 16; ++i) {
      ::base::StringAppendF(&text, "%02X", devinfo.serialnum[i]);
    }
    ::base::StringAppendF(&text, "\nFirmware Ver: %d.%02d\nHardware Rev: %d",
                          devinfo.firmware_version >> 8,
                          devinfo.firmware_version & 0xFF,
                          devinfo.hardware_version);
    LOG(INFO) << text;
  } else {
    return errors::Unavailable(
        ::base::StringPrintf("Failed to getDeviceInfo : %x", result));
  }

  rplidar_response_device_health_t healthinfo;
  result = driver->getHealth(healthinfo);
  if (IS_OK(result)) {
    if (healthinfo.status == RPLIDAR_STATUS_WARNING) {
      LOG(WARNING) << "getHealth " << healthinfo.error_code;
    } else if (healthinfo.status == RPLIDAR_STATUS_ERROR) {
      return errors::Unavailable(
          ::base::StringPrintf("rplidar internal error detected (%x). Please "
                               "reboot the device to retry.",
                               healthinfo.error_code));
    }
  } else {
    return errors::Unavailable(
        ::base::StringPrintf("Failed to getHealth : %x", result));
  }

  driver_ = std::move(driver);
  lidar_state_.ToInitialized();
  return Status::OK();
}

Status RPlidar::Start() {
  if (!lidar_state_.IsInitialized()) {
    return lidar_state_.InvalidStateError();
  }

  driver_->startMotor();
  driver_->startScan(0, 1);

  lidar_state_.ToStarted();
  return Status::OK();
}

Status RPlidar::Stop() {
  if (!lidar_state_.IsStarted()) {
    return lidar_state_.InvalidStateError();
  }

  driver_->stop();
  driver_->stopMotor();

  lidar_state_.ToStopped();
  return Status::OK();
}

}  // namespace felicia