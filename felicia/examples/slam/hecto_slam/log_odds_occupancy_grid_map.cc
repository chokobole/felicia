#include "felicia/examples/slam/hecto_slam/log_odds_occupancy_grid_map.h"

namespace felicia {

LogOddsCell::LogOddsCell() : value_(0) {}

bool LogOddsCell::IsOccupied() const { return value_ > 0; }

bool LogOddsCell::IsFree() const { return value_ < 0; }

void LogOddsCell::MarkOccupied(float v) {
  if (value_ < 50.0f) {
    value_ += v;
  }
}

void LogOddsCell::MarkFree(float v) { value_ += v; }

float LogOddsCell::Value() const {
  float odds = exp(value_);
  float value = odds / (odds + 1.0f);
  CHECK(!std::isnan(value));
  return value;
}

float LogOddsCell::ValueInternal() const { return value_; }

LogOddsCellContainer::LogOddsCellContainer(Sizei size)
    : size_(size),
      cells_(std::unique_ptr<LogOddsCell[]>(
          new LogOddsCell[static_cast<size_t>(size.area())])) {}

Sizei LogOddsCellContainer::Size() const { return size_; }

bool LogOddsCellContainer::HasCell(int x, int y) const {
  return x >= 0 && x < size_.width() && y >= 0 && y < size_.height();
}

const LogOddsCell& LogOddsCellContainer::At(int x, int y) const {
  size_t idx = y * size_.width() + x;
  return cells_[idx];
}

LogOddsCell& LogOddsCellContainer::At(int x, int y) {
  size_t idx = y * size_.width() + x;
  return cells_[idx];
}

const LogOddsCell* LogOddsCellContainer::cells() const { return cells_.get(); }

LogOddsOccupancyGridMap::LogOddsOccupancyGridMap(
    std::unique_ptr<LogOddsCellContainer> cell_container, float resolution,
    const Pointf& origin, float occupied_factor, float free_factor)
    : OccupancyGridMap<LogOddsCell>(std::move(cell_container)),
      occupied_factor_(ToLogOdds(occupied_factor)),
      free_factor_(ToLogOdds(free_factor)) {
  DCHECK_GT(resolution, 0.f);
  set_resolution(resolution);
  set_origin(origin * -1.f);
  float scale_to_map = 1 / resolution;
  Transformf world_to_map;
  world_to_map.AddScale(scale_to_map, scale_to_map)
      .AddTranslate(origin.x(), origin.y());
  set_world_to_map(world_to_map);
}

bool LogOddsOccupancyGridMap::IsOccupied(int x, int y) const {
  DCHECK(HasValue(x, y));
  return this->cell_container_->At(x, y).IsOccupied();
}

bool LogOddsOccupancyGridMap::IsFree(int x, int y) const {
  DCHECK(HasValue(x, y));
  return this->cell_container_->At(x, y).IsFree();
}

void LogOddsOccupancyGridMap::MarkOccupied(int x, int y) {
  DCHECK(HasValue(x, y));
  this->cell_container_->At(x, y).MarkOccupied(occupied_factor_);
}

void LogOddsOccupancyGridMap::MarkFree(int x, int y) {
  DCHECK(HasValue(x, y));
  this->cell_container_->At(x, y).MarkFree(free_factor_);
}

bool LogOddsOccupancyGridMap::HasValue(int x, int y) const {
  return this->cell_container_->HasCell(x, y);
}

float LogOddsOccupancyGridMap::Value(int x, int y) const {
  DCHECK(HasValue(x, y));
  return this->cell_container_->At(x, y).Value();
}

void LogOddsOccupancyGridMap::SetOccupiedFactor(float prob) {
  occupied_factor_ = ToLogOdds(prob);
}

void LogOddsOccupancyGridMap::SetFreeFactor(float prob) {
  free_factor_ = ToLogOdds(prob);
}

float LogOddsOccupancyGridMap::ToLogOdds(float prob) const {
  float odds = prob / (1.0f - prob);
  return log(odds);
}

const LogOddsCell* LogOddsOccupancyGridMap::cells() const {
  return reinterpret_cast<LogOddsCellContainer*>(cell_container_.get())
      ->cells();
}

std::string LogOddsOccupancyGridMap::ToMapData() const {
  std::string data;
  size_t size = this->area();
  data.resize(size);
  const LogOddsCell* cells = this->cells();
  for (size_t i = 0; i < size; ++i) {
    if (cells[i].IsOccupied()) {
      data[i] = 100;
    } else if (cells[i].IsFree()) {
      data[i] = 0;
    } else {
      data[i] = 101;
    }
  }

  return data;
}

}  // namespace felicia