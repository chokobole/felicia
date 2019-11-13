#ifndef FELICIA_MAP_POINTCLOUD_H_
#define FELICIA_MAP_POINTCLOUD_H_

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/containers/data.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/core/lib/unit/ui/color.h"
#include "felicia/map/map_message.pb.h"

namespace felicia {
namespace map {

class FEL_EXPORT Pointcloud {
 public:
  enum SerializeOption {
    WITH_INTENCITIES = 1,
    WITH_COLORS = 2,
  };

  Pointcloud();
  Pointcloud(const Data& points, const Data& intencities, const Data& colors,
             base::TimeDelta timestamp);
  Pointcloud(Data&& points, Data&& intencities, Data&& colors,
             base::TimeDelta timestamp) noexcept;
  Pointcloud(const Pointcloud& other);
  Pointcloud(Pointcloud&& other) noexcept;

  Pointcloud& operator=(const Pointcloud& other);
  Pointcloud& operator=(Pointcloud&& other);

  ~Pointcloud();

  const Data& points() const;
  Data& points();
  const Data& intencities() const;
  Data& intencities();
  const Data& colors() const;
  Data& colors();

  void set_timestamp(base::TimeDelta time);
  base::TimeDelta timestamp() const;

  PointcloudMessage ToPointcloudMessage(bool copy = true,
                                        int option = WITH_INTENCITIES |
                                                     WITH_COLORS);
  Status FromPointcloudMessage(const PointcloudMessage& message,
                               int option = WITH_INTENCITIES | WITH_COLORS);
  Status FromPointcloudMessage(PointcloudMessage&& message,
                               int option = WITH_INTENCITIES | WITH_COLORS);

  Status Load(const base::FilePath& path,
              int option = WITH_INTENCITIES | WITH_COLORS);
  Status Save(const base::FilePath& path,
              int option = WITH_INTENCITIES | WITH_COLORS) const;

 private:
  Data points_;
  Data intencities_;
  Data colors_;
  base::TimeDelta timestamp_;
};

typedef base::RepeatingCallback<void(Pointcloud&&)> PointcloudCallback;

}  // namespace map
}  // namespace felicia

#endif  // FELICIA_MAP_POINTCLOUD_H_