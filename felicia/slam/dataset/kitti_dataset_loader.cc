#include "felicia/slam/dataset/kitti_dataset_loader.h"

#include <iomanip>
#include <sstream>

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/file/csv_reader.h"
#include "felicia/slam/dataset/dataset_utils.h"

namespace felicia {
namespace slam {

KittiDatasetLoader::KittiDatasetLoader(const base::FilePath& path_to_sequence,
                                       uint8_t sequence)
    : current_(0) {
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << base::NumberToString(sequence);
  base::FilePath path = path_to_sequence.AppendASCII(ss.str());
  calibs_path_ = path.AppendASCII("calib.txt");
  times_path_ = path.AppendASCII("times.txt");
  left_images_path_ = path.AppendASCII("image_0");
  right_images_path_ = path.AppendASCII("image_1");
}

StatusOr<KittiCalibData> KittiDatasetLoader::Init() {
  CsvReader reader;
  Status s = reader.Open(calibs_path_, " ");
  if (!s.ok()) return s;
  enum { P0, P1, P2, P3, TR };
  int cur = P0;
  KittiCalibData kitti_calib_data;
  while (!reader.eof()) {
    std::vector<std::string> items;
    if (reader.ReadItems(&items)) {
      if (cur <= P3 && items.size() != 13) {
        return errors::InvalidArgument(
            base::StringPrintf("Invalid projection matrix at line %s:%d",
                               calibs_path_.value().c_str(), cur + 1));
      }
      if ((cur == P0 && items[0] == "P0:") ||
          (cur == P1 && items[0] == "P1:") ||
          (cur == P2 && items[0] == "P2:") ||
          (cur == P3 && items[0] == "P3:")) {
        Eigen::Matrix<double, 3, 4> m;
        for (int i = 0; i < 12; ++i) {
          StatusOr<double> status_or =
              TryConvertToDouble(items[i + 1], calibs_path_, cur + 1);
          if (!status_or.ok()) return status_or.status();
          m(i / 4, i % 4) = status_or.ValueOrDie();
        }

        if (cur == P0) {
          kitti_calib_data.p0 = m;
        } else if (cur == P1) {
          kitti_calib_data.p1 = m;
        } else if (cur == P2) {
          kitti_calib_data.p2 = m;
        } else if (cur == P3) {
          kitti_calib_data.p3 = m;
        }
      }
    }
    ++cur;
  }
  return kitti_calib_data;
}

StatusOr<SensorData> KittiDatasetLoader::Next() {
  if (!times_reader_.IsOpened()) {
    times_reader_.set_option(BufferedReader::REMOVE_CR_OR_LF);
    Status s = times_reader_.Open(times_path_);
    if (!s.ok()) return s;
  }

  size_t current = current_++;
  SensorData sensor_data;
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(6) << current;
  sensor_data.set_left_image_filename(left_images_path_.AppendASCII(ss.str())
                                          .AddExtensionASCII(".png")
                                          .AsUTF8Unsafe());
  sensor_data.set_right_image_filename(right_images_path_.AppendASCII(ss.str())
                                           .AddExtensionASCII(".png")
                                           .AsUTF8Unsafe());
  std::string line;
  if (times_reader_.ReadLine(&line)) {
    StatusOr<double> status_or =
        TryConvertToDouble(line, times_path_, current_);
    if (!status_or.ok()) return status_or.status();
    sensor_data.set_timestamp(status_or.ValueOrDie());
  }
  return sensor_data;
}

bool KittiDatasetLoader::End() const { return times_reader_.eof(); }

}  // namespace slam
}  // namespace felicia