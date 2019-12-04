// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_MAP_POINTCLOUD_H_
#define FELICIA_MAP_POINTCLOUD_H_

#if defined(HAS_ROS)
#include <sensor_msgs/PointCloud2.h>
#endif

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
  Pointcloud(const Data& points, const Data& intensities, const Data& colors,
             base::TimeDelta timestamp);
  Pointcloud(Data&& points, Data&& intensities, Data&& colors,
             base::TimeDelta timestamp) noexcept;
  Pointcloud(const Pointcloud& other);
  Pointcloud(Pointcloud&& other) noexcept;

  Pointcloud& operator=(const Pointcloud& other);
  Pointcloud& operator=(Pointcloud&& other);

  ~Pointcloud();

  const Data& points() const;
  Data& points();
  const Data& intensities() const;
  Data& intensities();
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

#if defined(HAS_ROS)
  // Copy to |pointcloud|, belows should be filled outside this function.
  // pointcloud->is_dense
  // pointcloud->is_bigendian
  // pointcloud->header.frame_id
  //
  // Supported types are like below.
  // point: Pointi, Pointf, Pointd, Point3i, Point3f, Point3d
  // color: Color3u, Color3f, Color4u, Color4f
  // intensity: uint8_t, uint16_t
  bool ToRosPointcloud(sensor_msgs::PointCloud2* pointcloud,
                       int option = WITH_INTENCITIES | WITH_COLORS) const;

  // Copy from |pointcloud|, belows should be filled outside this function.
  // pointcloud->is_dense
  // pointcloud->is_bigendian
  // pointcloud->header.frame_id
  //
  // Supported types are like below.
  // point: Pointi, Pointf, Pointd, Point3i, Point3f, Point3d
  // color: Color3u, Color3f, Color4u, Color4f
  // intensity: uint8_t, uint16_t
  Status FromRosPointcloud(const sensor_msgs::PointCloud2& pointcloud,
                           int option = WITH_INTENCITIES | WITH_COLORS);
#endif  // defined(HAS_ROS)

  Status Load(const base::FilePath& path,
              int option = WITH_INTENCITIES | WITH_COLORS);
  Status Save(const base::FilePath& path,
              int option = WITH_INTENCITIES | WITH_COLORS) const;

 private:
  Data points_;
  Data intensities_;
  Data colors_;
  base::TimeDelta timestamp_;
};

typedef base::RepeatingCallback<void(Pointcloud&&)> PointcloudCallback;

}  // namespace map
}  // namespace felicia

#endif  // FELICIA_MAP_POINTCLOUD_H_