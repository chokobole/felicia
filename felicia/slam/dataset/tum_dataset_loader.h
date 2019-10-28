#ifndef FELICIA_SLAM_DATASET_TUM_DATASET_LOADER_H_
#define FELICIA_SLAM_DATASET_TUM_DATASET_LOADER_H_

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/file/csv_reader.h"
#include "felicia/core/util/dataset/dataset_loader.h"
#include "felicia/slam/dataset/sensor_data.h"
#include "felicia/slam/dataset/sensor_meta_data.h"

namespace felicia {
namespace slam {

// SensorData::DataType data_type = SensorData::DATA_TYPE_LEFT_CAMERA;
// TumDataSetLoader loader("/path/to/tum", data_type);
// StatusOr<SensorMetaData> sensor_meta_data = loader.Init();
// StatusOr<SensorData> sensor_data = loader.Next();
// For example, /path/to/tum should point to the
// /path/to/rgbd_data_set/freiburg1_rpy.
class EXPORT TumDatasetLoader
    : public DatasetLoader<SensorMetaData, SensorData>::Delegate {
 public:
  enum TumKind { FR1, FR2, FR3 };
  class State {
   public:
    State(SensorData::DataType data_type, TumKind tum_kind,
          const base::FilePath& path);

    SensorData::DataType data_type() const;

    StatusOr<SensorMetaData> Init();
    StatusOr<SensorData> Next();
    bool End() const;

   private:
    base::FilePath PathToData() const;
    int ColumnsForData() const;

    TumKind tum_kind_;
    SensorData::DataType data_type_;
    // path to root
    base::FilePath path_;
    // path to data list e.g) /path/to/rgb.txt
    base::FilePath path_to_data_;
    CsvReader data_reader_;
    size_t current_ = 0;

    DISALLOW_COPY_AND_ASSIGN(State);
  };

  // We guess TumKind using |path|.
  TumDatasetLoader(const base::FilePath& path, int data_types);
  TumDatasetLoader(const base::FilePath& path, TumKind tum_kind,
                   int data_types);

  // DatasetLoader<SensorMetaData, SensorData>::Delegate methods
  StatusOr<SensorMetaData> Init(int data_type) override;
  StatusOr<SensorData> Next(int data_type) override;
  bool End(int data_type) const override;

 private:
  void InitStates(const base::FilePath& path, TumKind tum_kind, int data_types);

  std::vector<std::unique_ptr<State>> states_;

  DISALLOW_COPY_AND_ASSIGN(TumDatasetLoader);
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_TUM_DATASET_LOADER_H_