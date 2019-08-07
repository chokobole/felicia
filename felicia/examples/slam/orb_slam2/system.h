/**
 * This file is part of ORB-SLAM2.
 *
 * Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University
 * of Zaragoza) For more information see <https://github.com/raulmur/ORB_SLAM2>
 *
 * ORB-SLAM2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ORB-SLAM2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FELICIA_EXAMPLES_SLAM_ORB_SLAM2_SYSTEM_H_
#define FELICIA_EXAMPLES_SLAM_ORB_SLAM2_SYSTEM_H_

#include "third_party/chromium/base/files/file_path.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/unit/geometry/pose.h"
#include "felicia/drivers/camera/camera_frame.h"

namespace felicia {
namespace orb_slam2 {

class System {
 public:
  // Input sensor
  enum SensorType {
    SENSOR_TYPE_MONOCULAR = 0,
    SENSOR_TYPE_STEREO = 1,
    SENSOR_TYPE_RGBD = 2
  };

  class Client {
   public:
    virtual ~Client() = default;

    virtual void OnPoseUpdated(const Posef& pose,
                               ::base::TimeDelta timestamp) = 0;
  };

  System(Client* client, const ::base::FilePath& voc_path,
         const ::base::FilePath& settings_path, SensorType sensor_type);

  void TrackMonocular(CameraFrame&& camera_frame);

 private:
  Client* client_;  // not owned;
  SensorType sensor_type_;
};

}  // namespace orb_slam2
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_ORB_SLAM2_SYSTEM_H_