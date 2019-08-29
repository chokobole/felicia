#ifndef FELICIA_DRIVERS_LIDRA_LIDRA_FRAME_H_
#define FELICIA_DRIVERS_LIDRA_LIDRA_FRAME_H_

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/containers/data.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/drivers/lidar/lidar_frame_message.pb.h"

namespace felicia {
namespace drivers {

class EXPORT LidarFrame {
 public:
  LidarFrame();
  LidarFrame(float angle_start, float angle_end, float angle_delta,
             float time_delta, float scan_time, float range_min,
             float range_max, const Data& ranges, const Data& intensities,
             base::TimeDelta timestamp);
  LidarFrame(float angle_start, float angle_end, float angle_delta,
             float time_delta, float scan_time, float range_min,
             float range_max, Data&& ranges, Data&& intensities,
             base::TimeDelta timestamp) noexcept;
  LidarFrame(const LidarFrame& other);
  LidarFrame(LidarFrame&& other) noexcept;
  LidarFrame& operator=(const LidarFrame& other);
  LidarFrame& operator=(LidarFrame&& other);
  ~LidarFrame();

  void set_angle_start(float angle_start);
  float angle_start() const;
  void set_angle_end(float angle_end);
  float angle_end() const;
  void set_angle_delta(float angle_delta);
  float angle_delta() const;
  void set_time_delta(float time_delta);
  float time_delta() const;
  void set_scan_time(float scan_time);
  float scan_time() const;
  void set_range_min(float range_min);
  float range_min() const;
  void set_range_max(float range_max);
  float range_max() const;
  void set_timestamp(base::TimeDelta timestamp);
  base::TimeDelta timestamp() const;

  Data& ranges();
  Data& intensities();
  const Data& ranges() const;
  const Data& intensities() const;

  LidarFrameMessage ToLidarFrameMessage(bool copy = true);
  Status FromLidarFrameMessage(const LidarFrameMessage& message);
  Status FromLidarFrameMessage(LidarFrameMessage&& message);

  void Project(std::vector<Pointf>* points, float user_range_min,
               float user_range_max) const;

 private:
  float angle_start_;
  float angle_end_;
  float angle_delta_;
  float time_delta_;
  float scan_time_;
  float range_min_;
  float range_max_;
  Data ranges_;
  Data intensities_;
  base::TimeDelta timestamp_;
};

typedef base::RepeatingCallback<void(LidarFrame&&)> LidarFrameCallback;

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_LIDRA_LIDRA_FRAME_H_