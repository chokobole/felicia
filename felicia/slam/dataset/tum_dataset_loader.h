#ifndef FELICIA_SLAM_DATASET_TUM_DATASET_LOADER_H_
#define FELICIA_SLAM_DATASET_TUM_DATASET_LOADER_H_

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/file/buffered_reader.h"
#include "felicia/core/lib/unit/geometry/pose.h"
#include "felicia/core/lib/unit/geometry/vector.h"
#include "felicia/core/util/dataset/dataset_loader.h"
#include "felicia/slam/dataset/rgbd_data.h"
#include "felicia/slam/types.h"

namespace felicia {
namespace slam {

struct TumCalibData {
  EigenCameraMatrixd K;
  EigenDistortionMatrixd D;
};

// TODO: We should read the image and give users a CameraFrame
// |rgbd_data|, |acceleration| and |ground_truth| are mutually exclusive.
struct TumData {
  RgbdData rgbd_data;
  Vector3f acceleration;
  Pose3f ground_truth;
  double timestamp;
};

// TumDataSetLoader loader("/path/to/tum", TumDataSetLoader::FR1, TumData::RGB);
// StatusOr<TumCalibData> status_or = loader.Init();
// StatusOr<TumData> status_or2 = loader.Next();
// For example, /path/to/tum points to the /path/to/rgbd_data_set/freiburg1_rpy.
// NOTE: We expect the name of associated text file to be "associated.txt".
class EXPORT TumDatasetLoader : DatasetLoader<TumCalibData, TumData>::Delegate {
 public:
  enum TumKind { FR1, FR2, FR3 };
  enum DataKind {
    RGB,
    DEPTH,
    RGBD,
    ACCELERATION,
    GROUND_TRUTH,
  };

  TumDatasetLoader(const base::FilePath& path, TumKind kind,
                   DataKind data_kind);

  // DatasetLoader<TumCalibData, TumData>::Delegate methods
  StatusOr<TumCalibData> Init() override;
  StatusOr<TumData> Next() override;
  bool End() const override;

 private:
  base::FilePath PathToData() const;

  base::FilePath path_;
  base::FilePath path_to_data_;
  BufferedReader reader_;
  TumKind kind_;
  DataKind data_kind_;
  size_t current_;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_TUM_DATASET_LOADER_H_