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

// TumDataSetLoader
//        loader("/path/to/tum", TumDataSetLoader::FR1, TumDataSetLoader::RGB);
// StatusOr<SensorMetaData> sensor_meta_data = loader.Init();
// StatusOr<SensorData> sensor_data = loader.Next();
// For example, /path/to/tum points to the /path/to/rgbd_data_set/freiburg1_rpy.
// NOTE: We expect the name of associated text file to be "associated.txt".
class EXPORT TumDatasetLoader
    : DatasetLoader<SensorMetaData, SensorData>::Delegate {
 public:
  enum TumKind { FR1, FR2, FR3 };
  enum DataKind {
    RGB,
    DEPTH,
    RGBD,
    ACCELERATION,
    GROUND_TRUTH,
  };

  // We guess TumKind using |path|.
  TumDatasetLoader(const base::FilePath& path, DataKind data_kind);
  TumDatasetLoader(const base::FilePath& path, TumKind kind,
                   DataKind data_kind);

  // DatasetLoader<SensorMetaData, SensorData>::Delegate methods
  StatusOr<SensorMetaData> Init() override;
  StatusOr<SensorData> Next() override;
  bool End() const override;

 private:
  base::FilePath PathToData() const;
  int ColumnsForData() const;

  // path to root
  base::FilePath path_;
  // path to data list e.g) /path/to/rgb.txt
  base::FilePath path_to_data_;
  CsvReader reader_;
  TumKind kind_;
  DataKind data_kind_;
  size_t current_;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_TUM_DATASET_LOADER_H_