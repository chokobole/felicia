#ifndef FELICIA_DRIVERS_IMU_IMU_FORMAT_H_
#define FELICIA_DRIVERS_IMU_IMU_FORMAT_H_

#include <vector>

#include "felicia/core/lib/base/export.h"
#include "felicia/drivers/imu/imu_format_message.pb.h"

namespace felicia {

class EXPORT ImuFormat {
public:
  ImuFormat();
  ImuFormat(float frame_rate);

  ImuFormat(const ImuFormat& other);
  ImuFormat& operator=(const ImuFormat& other);

  std::string ToString() const;

  float frame_rate() const;
  void set_frame_rate(float frame_rate);

  ImuFormatMessage ToImuFormatMessage() const;

  bool operator==(const ImuFormat& other);

 private:
  float frame_rate_ = 0;
};

EXPORT std::ostream& operator<<(std::ostream& os, const ImuFormat& imu_format);

using ImuFormats = std::vector<ImuFormat>;

// Compares the priority of the imu formats. Returns true if |lhs| is the
// preferred imu format in comparison with |rhs|. Returns false otherwise.
bool CompareCapability(const ImuFormat& requested, const ImuFormat& lhs,
                       const ImuFormat& rhs);

}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_IMU_FORMAT_H_