#ifndef FELICIA_DRIVERS_POINTCLOUD_POINTCLOUD_FRAME_H_
#define FELICIA_DRIVERS_POINTCLOUD_POINTCLOUD_FRAME_H_

#include "third_party/chromium/base/callback.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/containers/string_vector.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/core/lib/unit/ui/color.h"
#include "felicia/drivers/pointcloud/pointcloud_frame_message.pb.h"

namespace felicia {
namespace drivers {

class EXPORT PointcloudFrame {
 public:
  PointcloudFrame();
  PointcloudFrame(size_t points_size, size_t colors_size);
  PointcloudFrame(const std::string& points, const std::string& colors,
                  base::TimeDelta timestamp);
  PointcloudFrame(std::string&& points, std::string&& colors,
                  base::TimeDelta timestamp) noexcept;
  PointcloudFrame(const PointcloudFrame& other);
  PointcloudFrame(PointcloudFrame&& other) noexcept;

  PointcloudFrame& operator=(const PointcloudFrame& other);
  PointcloudFrame& operator=(PointcloudFrame&& other);

  void AddPoint(float x, float y, float z);
  void AddPoint(const Point3f& point);
  void AddColor(uint8_t r, uint8_t g, uint8_t b);
  void AddColor(const Color3u& color);
  void AddPointAndColor(float x, float y, float z, uint8_t r, uint8_t g,
                        uint8_t b);

  Point3f& PointAt(size_t idx);
  Color3u& ColorAt(size_t idx);
  const Point3f& PointAt(size_t idx) const;
  const Color3u& ColorAt(size_t idx) const;

  void set_timestamp(base::TimeDelta time);
  base::TimeDelta timestamp() const;

  PointcloudFrameMessage ToPointcloudFrameMessage(bool copy = true);
  Status FromPointcloudFrameMessage(const PointcloudFrameMessage& message);
  Status FromPointcloudFrameMessage(PointcloudFrameMessage&& message);

 private:
  StringVector points_;
  StringVector colors_;
  base::TimeDelta timestamp_;
};

typedef base::RepeatingCallback<void(PointcloudFrame&&)>
    PointcloudFrameCallback;

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_POINTCLOUD_POINTCLOUD_FRAME_H_