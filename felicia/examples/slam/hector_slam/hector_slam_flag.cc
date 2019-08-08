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

#include "felicia/examples/slam/hector_slam/hector_slam_flag.h"

namespace felicia {
namespace hector_slam {

HectorSlamFlag::HectorSlamFlag() {
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&map_resolution_, 0.025f));
    auto flag = builder.SetLongName("--map_resolution")
                    .SetHelp(
                        "The map resolution [m]. This is the length of a grid "
                        "cell edge. (default: 0.025)")
                    .Build();
    map_resolution_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    IntDefaultFlag::Builder builder(MakeValueStore(&map_size_, 1024));
    auto flag = builder.SetLongName("--map_size")
                    .SetHelp(
                        "The size [number of cells per axis] of the map. The "
                        "map is square and has (map_size * map_size) grid "
                        "cells. (default: 1024)")
                    .Build();
    map_size_flag_ = std::make_unique<IntDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&map_start_x_, 0.5f));
    auto flag = builder.SetLongName("--map_start_x")
                    .SetHelp(
                        "Location of the origin [0.0, 1.0] of the /map frame "
                        "on the x axis relative to the grid map. 0.5 is in the "
                        "middle. (default: 0.5)")
                    .Build();
    map_start_x_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&map_start_y_, 0.5f));
    auto flag = builder.SetLongName("--map_start_y")
                    .SetHelp(
                        "Location of the origin [0.0, 1.0] of the /map frame "
                        "on the y axis relative to the grid map. 0.5 is in the "
                        "middle. (default: 0.5)")
                    .Build();
    map_start_y_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    IntDefaultFlag::Builder builder(MakeValueStore(&map_multi_res_levels_, 3));
    auto flag =
        builder.SetLongName("--map_multi_res_levels")
            .SetHelp(
                "The number of map multi-resolution grid levels. (default :3)")
            .Build();
    map_multi_res_levels_flag_ = std::make_unique<IntDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(
        MakeValueStore(&map_update_distance_thresh_, 0.4f));
    auto flag =
        builder.SetLongName("--map_update_distance_thresh")
            .SetHelp(
                "Threshold for performing map updates [m]. The platform has to "
                "travel this far in meters or experience an angular change as "
                "described by the map_update_angle_thresh parameter since the "
                "last update before a map update happens. (default: 0.4)")
            .Build();
    map_update_distance_thresh_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(
        MakeValueStore(&map_update_angle_thresh_, 0.9f));
    auto flag =
        builder.SetLongName("--map_update_angle_thresh")
            .SetHelp(
                "Threshold for performing map updates [rad]. The platform has "
                "to experience an angular change as described by this "
                "parameter of travel as far as specified by the "
                "map_update_distance_thresh parameter since the last update "
                "before a map update happens. (default: 0.9)")
            .Build();
    map_update_angle_thresh_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(
        MakeValueStore(&update_factor_free_, 0.4f));
    auto flag = builder.SetLongName("--update_factor_free")
                    .SetHelp(
                        "The map update modifier for updates of free cells in "
                        "the range [0.0, 1.0]. A value of 0.5 means no change. "
                        "(default: 0.4)")
                    .Build();
    update_factor_free_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(
        MakeValueStore(&update_factor_occupied_, 0.9f));
    auto flag = builder.SetLongName("--update_factor_occupied")
                    .SetHelp(
                        "The map update modifier for updates of occupied cells "
                        "in the range [0.0, 1.0]. A value of 0.5 means no "
                        "change. (default 0.9)")
                    .Build();
    update_factor_occupied_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&laser_min_dist_, 0.4f));
    auto flag = builder.SetLongName("--laser_min_dist")
                    .SetHelp(
                        "The minimum distance [m] for laser scan endpoints to "
                        "be used by the system. Scan endpoints closer than "
                        "this value are ignored. (default: 0.4)")
                    .Build();
    laser_min_dist_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&laser_max_dist_, 30.0f));
    auto flag = builder.SetLongName("--laser_max_dist")
                    .SetHelp(
                        "The maximum distance [m] for laser scan endpoints to "
                        "be used by the system. Scan endpoints farther away "
                        "than this value are ignored. (default: 30.0)")
                    .Build();
    laser_max_dist_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
}

HectorSlamFlag::~HectorSlamFlag() = default;

bool HectorSlamFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(
      parser, map_resolution_flag_, map_size_flag_, map_start_x_flag_,
      map_start_y_flag_, map_multi_res_levels_flag_,
      map_update_distance_thresh_flag_, map_update_angle_thresh_flag_,
      update_factor_free_flag_, update_factor_occupied_flag_,
      laser_min_dist_flag_, laser_max_dist_flag_);
}

bool HectorSlamFlag::Validate() const {
  return CheckIfFlagPositive(map_multi_res_levels_flag_);
}

std::vector<std::string> HectorSlamFlag::CollectUsages() const {
  std::vector<std::string> usages;
  usages.push_back("[--help]");
  AddUsage(usages, map_resolution_flag_, map_size_flag_, map_start_x_flag_,
           map_start_y_flag_, map_multi_res_levels_flag_,
           map_update_distance_thresh_flag_, map_update_angle_thresh_flag_,
           update_factor_free_flag_, update_factor_occupied_flag_,
           laser_min_dist_flag_, laser_max_dist_flag_);
  return usages;
}

std::string HectorSlamFlag::Description() const { return "HectorSLAM flags"; }

std::vector<NamedHelpType> HectorSlamFlag::CollectNamedHelps() const {
  return {
      std::make_pair(
          kYellowOptions,
          std::vector<std::string>{
              map_resolution_flag_->help(), map_size_flag_->help(),
              map_start_x_flag_->help(), map_start_y_flag_->help(),
              map_multi_res_levels_flag_->help(),
              map_update_distance_thresh_flag_->help(),
              map_update_angle_thresh_flag_->help(),
              update_factor_free_flag_->help(),
              update_factor_occupied_flag_->help(),
              laser_min_dist_flag_->help(), laser_max_dist_flag_->help()}),
  };
}

}  // namespace hector_slam
}  // namespace felicia