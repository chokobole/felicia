//=================================================================================================
// Copyright (c) 2011, Stefan Kohlbrecher, TU Darmstadt
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Simulation, Systems Optimization and Robotics
//       group, TU Darmstadt nor the names of its contributors may be used to
//       endorse or promote products derived from this software without
//       specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//=================================================================================================

#ifndef FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_NODE_H_
#define FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/communication/subscriber.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/lidar/lidar_frame.h"
#include "felicia/examples/slam/hector_slam/hector_slam.h"
#include "felicia/examples/slam/slam_node_create_flag.h"

namespace felicia {
namespace hector_slam {

class HectorSlamNode : public NodeLifecycle, public HectorSlam::Client {
 public:
  explicit HectorSlamNode(const SlamNodeCreateFlag& slam_node_create_flag);

  // NodeLifecycle methods
  void OnInit() override;
  void OnDidCreate(const NodeInfo& node_info) override;

  // HectorSlam::Client methods
  void OnPoseUpdated(const Posef& pose, base::TimeDelta timestamp) override;
  void OnMapUpdated(const MultiResolutionGridMap& map,
                    base::TimeDelta timestamp) override;

 private:
  void OnMessage(drivers::LidarFrameMessage&& message);

  void RequestSubscribe();
  void RequestPublish();

  NodeInfo node_info_;
  const SlamNodeCreateFlag& slam_node_create_flag_;
  const std::string lidar_topic_;
  const std::string map_topic_;
  const std::string pose_topic_;
  const float fps_;
  std::unique_ptr<HectorSlam> hector_slam_;
  Subscriber<drivers::LidarFrameMessage> lidar_subscriber_;
  Publisher<PosefWithTimestampMessage> pose_publisher_;
  Publisher<map::OccupancyGridMapMessage> map_publisher_;
};

}  // namespace hector_slam
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_NODE_H_