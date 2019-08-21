#include "felicia/slam/dataset/kitti_dataset_loader.h"

#include <iomanip>
#include <sstream>

#include "third_party/chromium/base/strings/string_tokenizer.h"
#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/strings/str_util.h"

namespace felicia {
namespace slam {

KittiDatasetLoader::KittiDatasetLoader(const base::FilePath& path_to_sequence,
                                       uint8_t sequence)
    : current_(0) {
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << base::NumberToString(sequence);
  path_ = path_to_sequence.AppendASCII(ss.str());
}

StatusOr<KittiCalibData> KittiDatasetLoader::Init() {
  BufferedReader reader;
  Status s = reader.Open(path_.AppendASCII("calib.txt"));
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
        line = line.substr(4, line.length() - 5);  // subtract \n
        base::StringTokenizer t(line, " ");
        Eigen::Matrix<double, 3, 4> m;
        int idx = 0;
        while (t.GetNext()) {
          double n;
          if (!base::StringToDouble(t.token(), &n)) {
            return errors::InvalidArgument(
                base::StringPrintf("Cann't convert to double %s at line %d",
                                   t.token().c_str(), cur + 1));
          }
          m(idx / 4, idx % 4) = n;
          ++idx;
        }
        if (idx != 12) {
          return errors::InvalidArgument(base::StringPrintf(
              "Invalid projection matrix at line %d", cur + 1));
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
    Status s = times_reader_.Open(path_.AppendASCII("times.txt"));
    if (!s.ok()) return s;
  }
  if (End()) return errors::OutOfRange("No data any more.");

  size_t current = current_++;
  KittiData kitti_data;
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(6) << current;
  kitti_data.left_image_filename = path_.AppendASCII("image_0")
                                       .AppendASCII(ss.str())
                                       .AddExtensionASCII(".png")
                                       .AsUTF8Unsafe();
  kitti_data.right_image_filename = path_.AppendASCII("image_1")
                                        .AppendASCII(ss.str())
                                        .AddExtensionASCII(".png")
                                        .AsUTF8Unsafe();
  std::string line;
  if (times_reader_.ReadLine(&line)) {
    double timestamp;
    if (!base::StringToDouble(line.substr(0, line.length() - 1), &timestamp)) {
      return errors::InvalidArgument(base::StringPrintf(
          "Cann't convert to double %s at line %zd", line.c_str(), current_));
    }
    kitti_data.timestamp = timestamp;
  }
  return kitti_data;
}

bool KittiDatasetLoader::End() const { return times_reader_.eof(); }

size_t KittiDatasetLoader::length() const { return 0; }

}  // namespace slam
}  // namespace felicia