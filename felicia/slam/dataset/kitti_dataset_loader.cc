#include "felicia/slam/dataset/kitti_dataset_loader.h"

#include <iomanip>
#include <sstream>

#include "third_party/chromium/base/strings/string_tokenizer.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/strings/str_util.h"
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
  BufferedReader reader(BufferedReader::REMOVE_CR_OR_LF);
  Status s = reader.Open(calibs_path_);
  if (!s.ok()) return s;
  enum { P0, P1, P2, P3, TR };
  int cur = P0;
  KittiCalibData kitti_calib_data;
  while (!reader.eof()) {
    std::string line;
    if (reader.ReadLine(&line)) {
      base::StringPiece text = line;
      if ((cur == P0 && StartsWith(text, "P0: ")) ||
          (cur == P1 && StartsWith(text, "P1: ")) ||
          (cur == P2 && StartsWith(text, "P2: ")) ||
          (cur == P3 && StartsWith(text, "P3: "))) {
        line = line.substr(4, line.length() - 4);
        base::StringTokenizer t(line, " ");
        Eigen::Matrix<double, 3, 4> m;
        int idx = 0;
        while (t.GetNext()) {
          StatusOr<double> status_or =
              TryConvertToDouble(t.token(), calibs_path_, cur + 1);
          if (!status_or.ok()) return status_or.status();
          m(idx / 4, idx % 4) = status_or.ValueOrDie();
          ++idx;
        }
        if (idx != 12) {
          return errors::InvalidArgument(
              base::StringPrintf("Invalid projection matrix at line %s:%d",
                                 calibs_path_.value().c_str(), cur + 1));
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

StatusOr<KittiData> KittiDatasetLoader::Next() {
  if (!times_reader_.IsOpened()) {
    times_reader_.set_option(BufferedReader::REMOVE_CR_OR_LF);
    Status s = times_reader_.Open(times_path_);
    if (!s.ok()) return s;
  }

  size_t current = current_++;
  KittiData kitti_data;
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(6) << current;
  kitti_data.stereo_data.left_image_filename =
      left_images_path_.AppendASCII(ss.str())
          .AddExtensionASCII(".png")
          .AsUTF8Unsafe();
  kitti_data.stereo_data.right_image_filename =
      right_images_path_.AppendASCII(ss.str())
          .AddExtensionASCII(".png")
          .AsUTF8Unsafe();
  std::string line;
  if (times_reader_.ReadLine(&line)) {
    StatusOr<double> status_or =
        TryConvertToDouble(line, times_path_, current_);
    if (!status_or.ok()) return status_or.status();
    kitti_data.timestamp = status_or.ValueOrDie();
  }
  return kitti_data;
}

bool KittiDatasetLoader::End() const { return times_reader_.eof(); }

}  // namespace slam
}  // namespace felicia