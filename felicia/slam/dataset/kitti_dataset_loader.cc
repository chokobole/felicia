#include "felicia/slam/dataset/kitti_dataset_loader.h"

#include <iomanip>
#include <sstream>

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/file/csv_reader.h"
#include "felicia/slam/dataset/dataset_utils.h"

namespace felicia {
namespace slam {

KittiDatasetLoader::KittiDatasetLoader(const base::FilePath& path,
                                       DataKind data_kind)
    : data_kind_(data_kind), current_(0) {
  calibs_path_ = path.AppendASCII("calib.txt");
  times_path_ = path.AppendASCII("times.txt");
  left_images_path_ = path.AppendASCII("image_0");
  right_images_path_ = path.AppendASCII("image_1");
}

StatusOr<SensorMetaData> KittiDatasetLoader::Init() {
  CsvReader reader;
  Status s = reader.Open(calibs_path_, " ");
  if (!s.ok()) return s;
  enum { P0, P1, P2, P3, TR };
  int cur = P0;
  SensorMetaData sensor_meta_data;
  while (!reader.eof()) {
    std::vector<std::string> rows;
    if (reader.ReadRows(&rows)) {
      if (cur <= P3 && rows.size() != 13) {
        return errors::InvalidArgument(base::StringPrintf(
            "Invalid projection matrix at line %" PRFilePath ":%d",
            calibs_path_.value().c_str(), cur + 1));
      }
      if (data_kind_ == GRAYSCALE && (cur >= P2 && cur <= P3)) continue;
      if (data_kind_ == COLOR && (cur >= P0 && cur <= P1)) continue;
      if ((cur == P0 && rows[0] == "P0:") || (cur == P1 && rows[0] == "P1:") ||
          (cur == P2 && rows[0] == "P2:") || (cur == P3 && rows[0] == "P3:")) {
        Eigen::Matrix<double, 3, 4> m;
        for (int i = 0; i < 12; ++i) {
          StatusOr<double> status_or =
              TryConvertToDouble(rows[i + 1], calibs_path_, cur + 1);
          if (!status_or.ok()) return status_or.status();
          m(i / 4, i % 4) = status_or.ValueOrDie();
        }

        if (cur == P0 || cur == P2) {
          sensor_meta_data.set_left_P(EigenProjectionMatrixd{m});
        } else if (cur == P1 || cur == P3) {
          sensor_meta_data.set_right_P(EigenProjectionMatrixd{m});
        }
      }
      // TOOD: Handle Tr
    }
    ++cur;
  }
  return sensor_meta_data;
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

bool KittiDatasetLoader::End() const {
  return times_reader_.IsOpened() && times_reader_.eof();
}

}  // namespace slam
}  // namespace felicia