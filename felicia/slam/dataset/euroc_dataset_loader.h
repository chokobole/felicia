#ifndef FELICIA_SLAM_DATASET_EUROC_DATASET_LOADER_H_
#define FELICIA_SLAM_DATASET_EUROC_DATASET_LOADER_H_

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/file/csv_reader.h"
#include "felicia/core/util/dataset/dataset_loader.h"
#include "felicia/drivers/imu/imu_filter_interface.h"
#include "felicia/slam/dataset/sensor_data.h"
#include "felicia/slam/dataset/sensor_meta_data.h"

namespace felicia {
namespace slam {

// SensorData::DataType data_type = SensorData::DATA_TYPE_LEFT_CAMERA;
// EurocDataSetLoader loader("/path/to/euroc", data_type);
// StatusOr<SensorMetaData> sensor_meta_data = loader.Init();
// StatusOr<SensorData> sensor_data = loader.Next();
// For example, /path/to/euroc should point to the /path/to/mav0.
class EXPORT EurocDatasetLoader
    : public DatasetLoader<SensorMetaData, SensorData>::Delegate {
 public:
  class State {
   public:
    State(SensorData::DataType data_type, const base::FilePath& path);

    SensorData::DataType data_type() const;

    StatusOr<SensorMetaData> Init();
    StatusOr<SensorData> Next();
    bool End() const;

   private:
    float LoadFrameRate();

    base::FilePath PathToMetaData() const;
    base::FilePath PathToData() const;
    base::FilePath PathToDataList() const;
    base::FilePath PathToDataType() const;
    int ColumnsForData() const;

    SensorData::DataType data_type_;
    // path to root
    base::FilePath path_;
    // path to root of each data e.g) /path/to/cam0/data
    base::FilePath path_to_data_;
    // path to data list e.g) /path/to/cam0/data.csv
    base::FilePath path_to_data_list_;
    CsvReader data_list_reader_;
    size_t current_ = 0;
    float frame_rate_ = -1;
    std::unique_ptr<drivers::ImuFilterInterface> imu_filter_;

    DISALLOW_COPY_AND_ASSIGN(State);
  };

  EurocDatasetLoader(const base::FilePath& path, int data_types);

  // DatasetLoader<SensorMetaData, SensorData>::Delegate methods
  StatusOr<SensorMetaData> Init(int data_type) override;
  StatusOr<SensorData> Next(int data_type) override;
  bool End(int data_type) const override;

 private:
  std::vector<std::unique_ptr<State>> states_;

  DISALLOW_COPY_AND_ASSIGN(EurocDatasetLoader);
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_EUROC_DATASET_LOADER_H_