#ifndef FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_LOG_ODDS_OCCUPANCY_GRID_MAP_H_
#define FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_LOG_ODDS_OCCUPANCY_GRID_MAP_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/slam/map/occupancy_grid_map.h"

namespace felicia {

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

class LogOddsCellContainer : public GridMap<LogOddsCell>::CellContainer {
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

class LogOddsOccupancyGridMap : public OccupancyGridMap<LogOddsCell> {
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

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_LOG_ODDS_OCCUPANCY_GRID_MAP_H_