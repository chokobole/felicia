#include "felicia/drivers/lidar/lidar_endpoint.h"

#include "third_party/chromium/base/strings/stringprintf.h"

namespace felicia {

LidarEndpoint::LidarEndpoint() = default;
LidarEndpoint::~LidarEndpoint() = default;

LidarEndpoint::LidarEndpoint(net::IPEndPoint ip_endpoint)
    : type_(ChannelType::TCP), ip_endpoint_(ip_endpoint) {}

LidarEndpoint::LidarEndpoint(const std::string& serial_port, float baudrate)
    : type_(ChannelType::SERIAL),
      serial_port_(serial_port),
      baudrate_(baudrate) {}

LidarEndpoint::LidarEndpoint(const LidarEndpoint& other) = default;
LidarEndpoint& LidarEndpoint::operator=(const LidarEndpoint& other) = default;

std::string LidarEndpoint::ToString() const {
  if (type_ == LidarEndpoint::TCP) {
    return base::StringPrintf("ip: %s port: %d", ip().c_str(), tcp_port());
  } else {
    return base::StringPrintf("port: %s baudrate: %.2f", serial_port_.c_str(),
                              baudrate_);
  }
}

LidarEndpoint::ChannelType LidarEndpoint::type() const { return type_; }

std::string LidarEndpoint::ip() const {
  return ip_endpoint_.address().ToString();
}

uint16_t LidarEndpoint::tcp_port() const { return ip_endpoint_.port(); }

const std::string& LidarEndpoint::serial_port() const { return serial_port_; }

float LidarEndpoint::buadrate() const { return baudrate_; }

std::ostream& operator<<(std::ostream& os,
                         const LidarEndpoint& rplidar_endpoint) {
  os << rplidar_endpoint.ToString();
  return os;
}

}  // namespace felicia