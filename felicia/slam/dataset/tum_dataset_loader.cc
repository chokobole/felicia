#include "felicia/slam/dataset/tum_dataset_loader.h"

#include "third_party/chromium/base/strings/string_tokenizer.h"

#include "felicia/slam/dataset/dataset_utils.h"

namespace felicia {
namespace slam {

TumDatasetLoader::TumDatasetLoader(const base::FilePath& path, TumKind kind,
                                   DataKind data_kind)
    : path_(path),
      kind_(kind),
      data_kind_(data_kind),
      current_(0) {
  path_to_data_ = PathToData();
}

StatusOr<TumCalibData> TumDatasetLoader::Init() {
  TumCalibData calib_data;
  if (kind_ == FR1) {
    calib_data.K = EigenCameraMatrixd(517.3, 516.5, 318.6, 255.3);
    calib_data.D =
        EigenDistortionMatrixd(0.2624, -0.9531, -0.0054, 0.0026, 1.1633);
  } else if (kind_ == FR2) {
    calib_data.K = EigenCameraMatrixd(520.9, 521.0, 325.1, 249.7);
    calib_data.D =
        EigenDistortionMatrixd(0.2312, -0.7849, -0.0033, -0.0001, 0.9172);
  } else if (kind_ == FR3) {
    calib_data.K = EigenCameraMatrixd(535.4, 539.2, 320.1, 247.6);
    calib_data.D = EigenDistortionMatrixd(0.0, 0.0, 0.0, 0.0, 0.0);
  }

  return calib_data;
}

StatusOr<TumData> TumDatasetLoader::Next() {
  if (!reader_.IsOpened()) {
    reader_.set_option(BufferedReader::REMOVE_CR_OR_LF);
    Status s = reader_.Open(path_to_data_);
    if (!s.ok()) return s;
  }

  if (data_kind_ != RGBD) {
    // Read comments
    for (int i = 0; i < 3; ++i) {
      std::string line;
      reader_.ReadLine(&line);
    }
  }

  ++current_;
  int current_line = current_ + 3;
  std::string line;
  TumData tum_data;
  if (reader_.ReadLine(&line)) {
    base::StringTokenizer t(line, " ");
    t.GetNext();
    StatusOr<double> status_or =
        TryConvertToDouble(t.token(), path_to_data_, current_line);
    if (!status_or.ok()) return status_or.status();
    tum_data.timestamp = status_or.ValueOrDie();
    switch (data_kind_) {
      case RGB: {
        t.GetNext();
        tum_data.rgbd_data.color_image_filename =
            path_.AppendASCII(t.token()).value();
        break;
      }
      case DEPTH: {
        t.GetNext();
        tum_data.rgbd_data.depth_image_filename =
            path_.AppendASCII(t.token()).value();
        break;
      }
      case RGBD: {
        t.GetNext();
        tum_data.rgbd_data.color_image_filename =
            path_.AppendASCII(t.token()).value();
        t.GetNext();
        t.GetNext();
        tum_data.rgbd_data.depth_image_filename =
            path_.AppendASCII(t.token()).value();
        break;
      }
      case ACCELERATION: {
        float v[3];
        for (int i = 0; i < 3; ++i) {
          t.GetNext();
          status_or =
              TryConvertToDouble(t.token(), path_to_data_, current_line);
          if (!status_or.ok()) return status_or.status();
          v[i] = status_or.ValueOrDie();
        }
        tum_data.acceleration.set_xyz(v[0], v[1], v[2]);
        break;
      }
      case GROUND_TRUTH: {
        float v[7];
        for (int i = 0; i < 7; ++i) {
          t.GetNext();
          status_or =
              TryConvertToDouble(t.token(), path_to_data_, current_line);
          if (!status_or.ok()) return status_or.status();
          v[i] = status_or.ValueOrDie();
        }
        Point3f p(v[0], v[1], v[2]);
        Quaternionf q(v[3], v[4], v[5], v[6]);
        tum_data.ground_truth = Pose3f{p, q};
        break;
      }
    }
  }
  return tum_data;
}

bool TumDatasetLoader::End() const { return reader_.eof(); }

base::FilePath TumDatasetLoader::PathToData() const {
  switch (data_kind_) {
    case RGB:
      return path_.AppendASCII("rgb.txt");
    case DEPTH:
      return path_.AppendASCII("depth.txt");
    case RGBD:
      return path_.AppendASCII("associated.txt");
    case ACCELERATION:
      return path_.AppendASCII("accelerometer.txt");
    case GROUND_TRUTH:
      return path_.AppendASCII("groundtruth.txt");
  }
  NOTREACHED();
  return path_;
}

}  // namespace slam
}  // namespace felicia