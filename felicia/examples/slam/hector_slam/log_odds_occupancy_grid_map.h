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

#ifndef FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_LOG_ODDS_OCCUPANCY_GRID_MAP_H_
#define FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_LOG_ODDS_OCCUPANCY_GRID_MAP_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/slam/map/occupancy_grid_map.h"

namespace felicia {
namespace hector_slam {

class LogOddsCell {
 public:
  LogOddsCell();

  bool IsOccupied() const;

  bool IsFree() const;

  void MarkOccupied(float v);

  void MarkFree(float v);

  float Value() const;

  float ValueInternal() const;

 private:
  float value_;
};

class LogOddsCellContainer : public slam::GridMap<LogOddsCell>::CellContainer {
 public:
  explicit LogOddsCellContainer(Sizei size);

  Sizei Size() const override;

  bool HasCell(int x, int y) const override;

  const LogOddsCell& At(int x, int y) const override;
  LogOddsCell& At(int x, int y) override;

  const LogOddsCell* cells() const;

 private:
  Sizei size_;
  std::unique_ptr<LogOddsCell[]> cells_;

  DISALLOW_COPY_AND_ASSIGN(LogOddsCellContainer);
};

class LogOddsOccupancyGridMap : public slam::OccupancyGridMap<LogOddsCell> {
 public:
  LogOddsOccupancyGridMap(std::unique_ptr<LogOddsCellContainer> cell_container,
                          float resolution, const Pointf& origin,
                          float occupied_factor = kDefaultOccupiedFactor,
                          float free_factor = kDefaultFreeFactor);

  // OccupancyGridMap<LogOddsCell> methods
  bool IsOccupied(int x, int y) const override;
  bool IsFree(int x, int y) const override;

  void MarkOccupied(int x, int y) override;
  void MarkFree(int x, int y) override;

  bool HasValue(int x, int y) const override;
  float Value(int x, int y) const override;

  std::string ToMapData() const override;

  void SetOccupiedFactor(float prob);
  void SetFreeFactor(float prob);

 private:
  float ToLogOdds(float prob) const;
  const LogOddsCell* cells() const;

  static constexpr float kDefaultOccupiedFactor = 0.6f;
  static constexpr float kDefaultFreeFactor = 0.4f;

  float occupied_factor_;
  float free_factor_;
};

}  // namespace hector_slam
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_LOG_ODDS_OCCUPANCY_GRID_MAP_H_