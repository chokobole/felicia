#ifndef FELICIA_DRIVERS_LIDAR_LIDAR_ENDPOINT_H_
#define FELICIA_DRIVERS_LIDAR_LIDAR_ENDPOINT_H_

#include "third_party/chromium/net/base/ip_endpoint.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

class EXPORT LidarEndpoint {
 public:
  enum ChannelType {
    TCP,
    SERIAL,
  };

  LidarEndpoint();
  ~LidarEndpoint();
  explicit LidarEndpoint(::net::IPEndPoint ip_endpoint);
  LidarEndpoint(const std::string& seiral_port, float baudrate);

  LidarEndpoint(const LidarEndpoint& other);
  LidarEndpoint& operator=(const LidarEndpoint& other);

  std::string ToString() const;

  ChannelType type() const;

  std::string ip() const;

  uint16_t tcp_port() const;

  const std::string& serial_port() const;

  float buadrate() const;

 private:
  ChannelType type_;
  ::net::IPEndPoint ip_endpoint_;
  std::string serial_port_;
  float baudrate_;
};

EXPORT std::ostream& operator<<(std::ostream& os,
                                const LidarEndpoint& lidar_endpoint);

}  // namespace felicia

#endif  // FELICIA_DRIVERS_LIDAR_LIDAR_ENDPOINT_H_