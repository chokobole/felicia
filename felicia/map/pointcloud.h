#ifndef FELICIA_MAP_POINTCLOUD_H_
#define FELICIA_MAP_POINTCLOUD_H_

#include "third_party/chromium/base/callback.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/containers/data.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/core/lib/unit/ui/color.h"
#include "felicia/map/map_message.pb.h"

namespace felicia {
namespace map {

class EXPORT Pointcloud {
 public:
  Pointcloud();
  Pointcloud(const Data& points, const Data& colors, base::TimeDelta timestamp);
  Pointcloud(Data&& points, Data&& colors, base::TimeDelta timestamp) noexcept;
  Pointcloud(const Pointcloud& other);
  Pointcloud(Pointcloud&& other) noexcept;

  Pointcloud& operator=(const Pointcloud& other);
  Pointcloud& operator=(Pointcloud&& other);

  const Data& points() const;
  Data& points();
  const Data& colors() const;
  Data& colors();

  void set_timestamp(base::TimeDelta time);
  base::TimeDelta timestamp() const;

  PointcloudMessage ToPointcloudMessage(bool copy = true);
  Status FromPointcloudMessage(const PointcloudMessage& message);
  Status FromPointcloudMessage(PointcloudMessage&& message);

 private:
  Data points_;
  Data colors_;
  base::TimeDelta timestamp_;
};

typedef base::RepeatingCallback<void(Pointcloud&&)> PointcloudCallback;

}  // namespace map
}  // namespace felicia

#endif  // FELICIA_MAP_POINTCLOUD_H_