#ifndef FELICIA_SLAM_DATASET_EUROC_DATASET_LOADER_H_
#define FELICIA_SLAM_DATASET_EUROC_DATASET_LOADER_H_

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/file/csv_reader.h"
#include "felicia/core/util/dataset/dataset_loader.h"
#include "felicia/slam/dataset/sensor_data.h"
#include "felicia/slam/dataset/sensor_meta_data.h"

namespace felicia {
namespace slam {

// EurocDataSetLoader loader("/path/to/euroc", EurocDataSetLoader::CAM0);
// StatusOr<SensorMetaData> status_or = loader.Init();
// StatusOr<SensorData> status_or2 = loader.Next();
// For example, /path/to/euroc points to the /path/to/mav0.
class EXPORT EurocDatasetLoader
    : DatasetLoader<SensorMetaData, SensorData>::Delegate {
 public:
  enum DataKind {
    CAM0,
    CAM1,
    IMU0,
    LEICA0,
    GROUND_TRUTH,
  };

  EurocDatasetLoader(const base::FilePath& path, DataKind data_kind);

  // DatasetLoader<SensorMetaData, SensorData>::Delegate methods
  StatusOr<SensorMetaData> Init() override;
  StatusOr<SensorData> Next() override;
  bool End() const override;

 private:
  base::FilePath PathToMetaData() const;
  base::FilePath PathToData() const;
  base::FilePath PathToDataList() const;
  base::FilePath PathToDataKind() const;
  int ColumnsForData() const;

  // path to root
  base::FilePath path_;
  // path to root of each data e.g) /path/to/cam0/data
  base::FilePath path_to_data_;
  // path to data list e.g) /path/to/cam0/data.csv
  base::FilePath path_to_data_list_;
  CsvReader reader_;
  DataKind data_kind_;
  size_t current_;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_EUROC_DATASET_LOADER_H_