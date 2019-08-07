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

#ifndef FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_FLAG_H_
#define FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_FLAG_H_

#include <memory>

#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {
namespace hector_slam {

class HectorSlamFlag : public FlagParser::Delegate {
 public:
  HectorSlamFlag();
  ~HectorSlamFlag();

  const FloatDefaultFlag* map_resolution_flag() const {
    return map_resolution_flag_.get();
  }
  const IntDefaultFlag* map_size_flag() const { return map_size_flag_.get(); }
  const FloatDefaultFlag* map_start_x_flag() const {
    return map_start_x_flag_.get();
  }
  const FloatDefaultFlag* map_start_y_flag() const {
    return map_start_y_flag_.get();
  }
  const IntDefaultFlag* map_multi_res_levels_flag() const {
    return map_multi_res_levels_flag_.get();
  }
  const FloatDefaultFlag* map_update_distance_thresh_flag() const {
    return map_update_distance_thresh_flag_.get();
  }
  const FloatDefaultFlag* map_update_angle_thresh_flag() const {
    return map_update_angle_thresh_flag_.get();
  }
  const FloatDefaultFlag* update_factor_free_flag() const {
    return update_factor_free_flag_.get();
  }
  const FloatDefaultFlag* update_factor_occupied_flag() const {
    return update_factor_occupied_flag_.get();
  }
  const FloatDefaultFlag* laser_min_dist_flag() const {
    return laser_min_dist_flag_.get();
  }
  const FloatDefaultFlag* laser_max_dist_flag() const {
    return laser_max_dist_flag_.get();
  }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  std::vector<std::string> CollectUsages() const override;
  std::string Description() const override;
  std::vector<NamedHelpType> CollectNamedHelps() const override;

 private:
  float map_resolution_;
  int map_size_;
  float map_start_x_;
  float map_start_y_;
  int map_multi_res_levels_;
  float map_update_distance_thresh_;
  float map_update_angle_thresh_;
  float update_factor_free_;
  float update_factor_occupied_;
  float laser_min_dist_;
  float laser_max_dist_;
  std::unique_ptr<FloatDefaultFlag> map_resolution_flag_;
  std::unique_ptr<IntDefaultFlag> map_size_flag_;
  std::unique_ptr<FloatDefaultFlag> map_start_x_flag_;
  std::unique_ptr<FloatDefaultFlag> map_start_y_flag_;
  std::unique_ptr<IntDefaultFlag> map_multi_res_levels_flag_;
  std::unique_ptr<FloatDefaultFlag> map_update_distance_thresh_flag_;
  std::unique_ptr<FloatDefaultFlag> map_update_angle_thresh_flag_;
  std::unique_ptr<FloatDefaultFlag> update_factor_free_flag_;
  std::unique_ptr<FloatDefaultFlag> update_factor_occupied_flag_;
  std::unique_ptr<FloatDefaultFlag> laser_min_dist_flag_;
  std::unique_ptr<FloatDefaultFlag> laser_max_dist_flag_;
};

}  // namespace hector_slam
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_HECTOR_SLAM_FLAG_H_