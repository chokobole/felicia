#ifndef FELICIA_DRIVERS_LIDRA_LIDRA_FRAME_H_
#define FELICIA_DRIVERS_LIDRA_LIDRA_FRAME_H_

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/drivers/lidar/lidar_frame_message.pb.h"

namespace felicia {

class EXPORT LidarFrame {
 public:
  LidarFrame();
  LidarFrame(float angle_start, float angle_end, float angle_delta,
             float time_delta, float scan_time, float range_min,
             float range_max, std::vector<float>&& ranges,
             std::vector<float> intensities,
             ::base::TimeDelta timestamp) noexcept;
  LidarFrame(LidarFrame&& other) noexcept;
  LidarFrame& operator=(LidarFrame&& other);

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
  void set_timestamp(::base::TimeDelta timestamp);
  ::base::TimeDelta timestamp() const;

  std::vector<float>& ranges();
  const std::vector<float>& ranges() const;
  std::vector<float>& intensities();
  const std::vector<float>& intensities() const;

  LidarFrameMessage ToLidarFrameMessage() const;
  static LidarFrame FromLidarFrameMessage(const LidarFrameMessage& message);

  void Project(std::vector<Pointf>* points);

 private:
  float angle_start_;
  float angle_end_;
  float angle_delta_;
  float time_delta_;
  float scan_time_;
  float range_min_;
  float range_max_;
  std::vector<float> ranges_;
  std::vector<float> intensities_;
  ::base::TimeDelta timestamp_;

  DISALLOW_COPY_AND_ASSIGN(LidarFrame);
};

typedef ::base::RepeatingCallback<void(const LidarFrame&)> LidarFrameCallback;

}  // namespace felicia

#endif  // FELICIA_DRIVERS_LIDRA_LIDRA_FRAME_H_