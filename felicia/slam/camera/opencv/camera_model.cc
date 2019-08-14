/*
Copyright (c) 2010-2016, Mathieu Labbe - IntRoLab - Universite de Sherbrooke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Universite de Sherbrooke nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "felicia/slam/camera/opencv/camera_model.h"

#include <opencv2/imgproc/imgproc.hpp>

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/math/matrix_util.h"
#include "felicia/core/lib/strings/str_util.h"

namespace felicia {
namespace slam {

namespace {

Status InvalidImageSize(int width, int height) {
  return errors::InvalidArgument(
      base::StringPrintf("image size is not valid. (%dx%d)", width, height));
}

bool IsValidImageSize(int width, int height) {
  return width >= 0 && height >= 0;
}

bool IsValidKMatrix(const cv::Mat1d& K) {
  return K.empty() || IsMatrix<3, 3>(K.rows, K.cols);
}

bool IsValidDMatrix(const cv::Mat1d& D) {
  return D.empty() || IsValidDistortionMatrix(D.rows, D.cols);
}

bool IsValidRMatrix(const cv::Mat1d& R) {
  return R.empty() || IsMatrix<3, 3>(R.rows, R.cols);
}

bool IsValidPMatrix(const cv::Mat1d& P) {
  return P.empty() || IsMatrix<3, 4>(P.rows, P.cols);
}

}  // namespace

namespace internal {

Status InvalidRowsAndCols(int rows, int cols) {
  return errors::InvalidArgument(
      base::StringPrintf("rows and cols are not invalid. (%dx%d)", rows, cols));
}

Status MaybeLoad(const cv::FileStorage& fs, const std::string& name,
                 const base::FilePath& path,
                 std::function<Status(const cv::FileNode&)> callback) {
  cv::FileNode n = fs[name];
  if (n.type() != cv::FileNode::NONE) {
    return callback(n);
  } else {
    LOG(WARNING) << "Missing \"" << name << "\" field in \"" << path << "\"";
  }
  return Status::OK();
}

StatusOr<cv::Mat1d> LoadCvMatrix(const cv::FileNode& n,
                                 std::function<bool(int, int)> callback) {
  int rows = static_cast<int>(n["rows"]);
  int cols = static_cast<int>(n["cols"]);
  std::vector<double> data;
  n["data"] >> data;
  if (rows * cols != static_cast<int>(data.size())) {
    return errors::InvalidArgument(base::StringPrintf(
        "rowsxcols are not matched with data size. %dx%d = %d", rows, cols,
        static_cast<int>(data.size())));
  }
  if (!callback(rows, cols)) {
    return internal::InvalidRowsAndCols(rows, cols);
  }
  return cv::Mat1d(rows, cols, data.data()).clone();
}
}  // namespace internal

CameraModel::CameraModel() = default;

CameraModel::CameraModel(const std::string& name, const cv::Size& image_size,
                         const cv::Mat1d& K, const cv::Mat1d& D,
                         const cv::Mat1d& R, const cv::Mat1d& P)
    : name_(name), image_size_(image_size), K_(K), D_(D), R_(R), P_(P) {
  CHECK(IsValidImageSize(image_size.width, image_size.height));
  CHECK(IsValidKMatrix(K));
  CHECK(IsValidDMatrix(D));
  CHECK(IsValidRMatrix(R));
  CHECK(IsValidPMatrix(P));

  if (IsValidForRectification()) {
    InitRectificationMap();
  }
}

CameraModel::CameraModel(double fx, double fy, double cx, double cy, double Tx,
                         const cv::Size& image_size)
    : CameraModel(base::EmptyString(), fx, fy, cx, cy, Tx, image_size) {}

CameraModel::CameraModel(const std::string& name, double fx, double fy,
                         double cx, double cy, double Tx,
                         const cv::Size& image_size)
    : name_(name), K_(cv::Mat1d::eye(3, 3)) {
  CHECK_GT(fx, 0.0);
  CHECK_GT(fy, 0.0);
  CHECK_GE(cx, 0.0);
  CHECK_GE(cy, 0.0);
  CHECK(IsValidImageSize(image_size.width, image_size.height));

  if (Tx != 0.0) {
    P_ = cv::Mat1d::eye(3, 4);
    P_(0, 0) = fx;
    P_(1, 1) = fy;
    P_(0, 3) = Tx;
  }

  cv::Mat1d& K = K_.matrix();
  K(0, 0) = fx;
  K(1, 1) = fy;

  set_image_size(image_size);
}

CameraModel::~CameraModel() = default;

void CameraModel::InitRectificationMap() {
  const cv::Mat1d& K = K_.matrix();
  const cv::Mat1d& D = D_.matrix();
  if (D.cols == 6) {
#if CV_MAJOR_VERSION > 2 or    \
    (CV_MAJOR_VERSION == 2 and \
     (CV_MINOR_VERSION > 4 or  \
      (CV_MINOR_VERSION == 4 and CV_SUBMINOR_VERSION >= 10)))
    // Equidistant / FishEye
    // get only k parameters (k1,k2,p1,p2,k3,k4)
    cv::Mat1d D2(1, 4);
    D2(0, 0) = D(0, 0);
    D2(0, 1) = D(0, 1);
    D2(0, 2) = D(0, 4);
    D2(0, 3) = D(0, 5);
    cv::fisheye::initUndistortRectifyMap(K, D2, R_, P_, image_size_, CV_32FC1,
                                         map_x_, map_y_);

  } else
#else
    NOTREACHED() << base::StringPrintf(
        "Too old opencv version (%d,%d,%d) to support fisheye model (min "
        "2.4.10 required)!",
        CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);
  }
#endif
  {
    // RadialTangential
    cv::initUndistortRectifyMap(K, D, R_, P_, image_size_, CV_32FC1, map_x_,
                                map_y_);
  }
}

void CameraModel::set_image_size(const cv::Size& size) {
  CHECK((size.height > 0 && size.width > 0) ||
        (size.height == 0 && size.width == 0));
  image_size_ = size;
  double ncx = cx();
  double ncy = cy();
  if (ncx == 0.0 && image_size_.width > 0) {
    ncx = static_cast<double>(image_size_.width) / 2.0 - 0.5;
  }
  if (ncy == 0.0 && image_size_.height > 0) {
    ncy = static_cast<double>(image_size_.height) / 2.0 - 0.5;
  }
  if (!P_.empty()) {
    P_(0, 2) = ncx;
    P_(1, 2) = ncy;
  }
  if (!K_.matrix().empty()) {
    K_.matrix()(0, 2) = ncx;
    K_.matrix()(1, 2) = ncy;
  }
}

Status CameraModel::Load(const base::FilePath& path) {
  try {
    cv::FileStorage fs(path.AsUTF8Unsafe(), cv::FileStorage::READ);

    std::string name = name_;
    cv::Size image_size = image_size_;
    cv::Mat1d K = K_.matrix();
    cv::Mat1d D = D_.matrix();
    cv::Mat1d R = R_;
    cv::Mat1d P = P_;

    Status s;

    internal::MaybeLoad(fs, "camera_name", path,
                        [&name](const cv::FileNode& n) {
                          name = static_cast<std::string>(n);
                          return Status::OK();
                        });
    internal::MaybeLoad(fs, "image_width", path,
                        [&image_size](const cv::FileNode& n) {
                          image_size.width = static_cast<int>(n);
                          return Status::OK();
                        });
    internal::MaybeLoad(fs, "image_height", path,
                        [&image_size](const cv::FileNode& n) {
                          image_size.height = static_cast<int>(n);
                          return Status::OK();
                        });
    if (!IsValidImageSize(image_size.width, image_size.height))
      return InvalidImageSize(image_size.width, image_size.height);

    // import from ROS calibration format
    s = internal::MaybeLoad(fs, "camera_matrix", path,
                            [&K](const cv::FileNode& n) {
                              StatusOr<cv::Mat1d> status_or =
                                  internal::LoadCvMatrix(n, IsMatrix<3, 3>);
                              if (!status_or.ok()) return status_or.status();
                              K = status_or.ValueOrDie();
                              return Status::OK();
                            });
    if (!s.ok()) return s;

    s = internal::MaybeLoad(
        fs, "distortion_coefficients", path, [&D](const cv::FileNode& n) {
          StatusOr<cv::Mat1d> status_or =
              internal::LoadCvMatrix(n, IsValidDistortionMatrix);
          if (!status_or.ok()) return status_or.status();
          D = status_or.ValueOrDie();
          return Status::OK();
        });
    if (!s.ok()) return s;

    internal::MaybeLoad(
        fs, "distortion_model", path, [&D](const cv::FileNode& n) {
          std::string distortionModel = static_cast<std::string>(n);
          if (D.cols >= 4 && (Contains(distortionModel, "fisheye") ||
                              Contains(distortionModel, "equidistant"))) {
            cv::Mat1d D2 = cv::Mat1d::zeros(1, 6);
            D2(0, 0) = D(0, 0);
            D2(0, 1) = D(0, 1);
            D2(0, 4) = D(0, 2);
            D2(0, 5) = D(0, 3);
            D = D2;
          }
          return Status::OK();
        });

    s = internal::MaybeLoad(fs, "rectification_matrix", path,
                            [&R](const cv::FileNode& n) {
                              StatusOr<cv::Mat1d> status_or =
                                  internal::LoadCvMatrix(n, IsMatrix<3, 3>);
                              if (!status_or.ok()) return status_or.status();
                              R = status_or.ValueOrDie();
                              return Status::OK();
                            });
    if (!s.ok()) return s;

    s = internal::MaybeLoad(fs, "projection_matrix", path,
                            [&P](const cv::FileNode& n) {
                              StatusOr<cv::Mat1d> status_or =
                                  internal::LoadCvMatrix(n, IsMatrix<3, 4>);
                              if (!status_or.ok()) return status_or.status();
                              P = status_or.ValueOrDie();
                              return Status::OK();
                            });
    if (!s.ok()) return s;

    fs.release();

    name_ = name;
    image_size_ = image_size;
    K_ = K;
    D_ = D;
    R_ = R;
    P_ = P;

    if (IsValidForRectification()) {
      InitRectificationMap();
    }

  } catch (const cv::Exception& e) {
    return errors::InvalidArgument(
        base::StringPrintf("Error reading calibration file %s : %s",
                           path.value().c_str(), e.what()));
  }

  return Status::OK();
}

Status CameraModel::Save(const base::FilePath& path) const {
  const cv::Mat1d& K = K_.matrix();
  const cv::Mat1d& D = D_.matrix();
  if (!K.empty() || !D.empty() || !R_.empty() || !P_.empty()) {
    try {
      cv::FileStorage fs(path.AsUTF8Unsafe(), cv::FileStorage::WRITE);

      // export in ROS calibration format
      if (!name_.empty()) {
        fs << "camera_name" << name_;
      }

      if (image_size_.width > 0 && image_size_.height > 0) {
        fs << "image_width" << image_size_.width;
        fs << "image_height" << image_size_.height;
      }

      if (!K.empty()) {
        fs << "camera_matrix"
           << "{";
        fs << "rows" << K.rows;
        fs << "cols" << K.cols;
        fs << "data"
           << std::vector<double>(
                  reinterpret_cast<double*>(K.data),
                  reinterpret_cast<double*>(K.data) + (K.rows * K.cols));
        fs << "}";
      }

      if (!D.empty()) {
        cv::Mat1d D2 = D;
        if (D.cols == 6) {
          D2 = cv::Mat1d(1, 4);
          D2(0, 0) = D(0, 0);
          D2(0, 1) = D(0, 1);
          D2(0, 2) = D(0, 4);
          D2(0, 3) = D(0, 5);
        }
        fs << "distortion_coefficients"
           << "{";
        fs << "rows" << D2.rows;
        fs << "cols" << D2.cols;
        fs << "data"
           << std::vector<double>(
                  reinterpret_cast<double*>(D2.data),
                  reinterpret_cast<double*>(D2.data) + (D2.rows * D2.cols));
        fs << "}";

        // compaibility with ROS
        if (D.cols == 6) {
          fs << "distortion_model"
             << "equidistant";  // equidistant, fisheye
        } else if (D2.cols > 5) {
          fs << "distortion_model"
             << "rational_polynomial";  // rad tan
        } else {
          fs << "distortion_model"
             << "plumb_bob";  // rad tan
        }
      }

      if (!R_.empty()) {
        fs << "rectification_matrix"
           << "{";
        fs << "rows" << R_.rows;
        fs << "cols" << R_.cols;
        fs << "data"
           << std::vector<double>(
                  reinterpret_cast<double*>(R_.data),
                  reinterpret_cast<double*>(R_.data) + (R_.rows * R_.cols));
        fs << "}";
      }

      if (!P_.empty()) {
        fs << "projection_matrix"
           << "{";
        fs << "rows" << P_.rows;
        fs << "cols" << P_.cols;
        fs << "data"
           << std::vector<double>(
                  reinterpret_cast<double*>(P_.data),
                  (reinterpret_cast<double*>(P_.data)) + (P_.rows * P_.cols));
        fs << "}";
      }

      fs.release();
    } catch (const cv::Exception& e) {
      return errors::InvalidArgument(
          base::StringPrintf("Error writing calibration file %s : %s",
                             path.value().c_str(), e.what()));
    }
  } else {
    return errors::InvalidArgument(
        "Cannot save calibration, check if K, D, R and P are not empty");
  }
  return Status::OK();
}

CameraModelMessage CameraModel::ToCameraModelMessage() const {
  CameraModelMessage message;
  SizeiMessage* image_size_message = message.mutable_image_size();
  image_size_message->set_width(image_size_.width);
  image_size_message->set_height(image_size_.height);
  const cv::Mat1d& K = K_.matrix();
  const cv::Mat1d& D = D_.matrix();
  if (!K.empty()) message.set_k(K.data, K.total() * sizeof(double));
  if (!D.empty()) message.set_d(D.data, D.total() * sizeof(double));
  if (!R_.empty()) message.set_r(R_.data, R_.total() * sizeof(double));
  if (!P_.empty()) message.set_p(P_.data, P_.total() * sizeof(double));

  return message;
}

Status CameraModel::FromCameraModelMessage(const CameraModelMessage& message) {
  cv::Size image_size = image_size_;
  cv::Mat1d K = K_.matrix();
  cv::Mat1d D = D_.matrix();
  cv::Mat1d R = R_;
  cv::Mat1d P = P_;

  if (message.has_image_size()) {
    int width = message.image_size().width();
    int height = message.image_size().height();
    if (!IsValidImageSize(width, height))
      return InvalidImageSize(width, height);
    image_size.width = width;
    image_size.height = height;
  }

  if (!message.k().empty()) {
    const std::string& k = message.k();
    int rows = 3;
    int cols = k.length() / rows;
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    K = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(k.c_str())))
            .clone();
  }

  if (!message.d().empty()) {
    const std::string& d = message.d();
    int rows = 1;
    int cols = d.length() / rows;
    if (!IsValidDistortionMatrix(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    D = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(d.c_str())))
            .clone();
  }

  if (!message.r().empty()) {
    const std::string& r = message.r();
    int rows = 3;
    int cols = r.length() / rows;
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    R = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(r.c_str())))
            .clone();
  }

  if (!message.p().empty()) {
    const std::string& p = message.p();
    int rows = 3;
    int cols = p.length() / rows;
    if (!IsMatrix<3, 4>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    P = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(p.c_str())))
            .clone();
  }

  image_size_ = image_size;
  K_ = K;
  D_ = D;
  R_ = R;
  P_ = P;
  return Status::OK();
}

Status CameraModel::FromCameraModelMessage(CameraModelMessage&& message) {
  cv::Size image_size = image_size_;
  cv::Mat1d K = K_.matrix();
  cv::Mat1d D = D_.matrix();
  cv::Mat1d R = R_;
  cv::Mat1d P = P_;

  if (message.has_image_size()) {
    int width = message.image_size().width();
    int height = message.image_size().height();
    if (!IsValidImageSize(width, height))
      return InvalidImageSize(width, height);
    image_size.width = width;
    image_size.height = height;
  }

  if (!message.k().empty()) {
    std::string* k = message.release_k();
    int rows = 3;
    int cols = k->length() / rows;
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    K = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(k->c_str())));
  }

  if (!message.d().empty()) {
    std::string* d = message.release_d();
    int rows = 1;
    int cols = d->length() / rows;
    if (!IsValidDistortionMatrix(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    D = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(d->c_str())));
  }

  if (!message.r().empty()) {
    std::string* r = message.release_r();
    int rows = 3;
    int cols = r->length() / rows;
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    R = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(r->c_str())));
  }

  if (!message.p().empty()) {
    std::string* p = message.release_p();
    int rows = 3;
    int cols = p->length() / rows;
    if (!IsMatrix<3, 4>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    P = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(p->c_str())));
  }

  image_size_ = image_size;
  K_ = K;
  D_ = D;
  R_ = R;
  P_ = P;
  return Status::OK();
}

CameraModel CameraModel::Scaled(double scale) const {
  CameraModel scaled_model = *this;
  CHECK_GT(scale, 0.0);
  if (IsValidForProjection()) {
    // has only effect on K and P
    cv::Mat1d K;
    if (!K_.matrix().empty()) {
      K = K_.matrix().clone();
      K(0, 0) *= scale;
      K(1, 1) *= scale;
      K(0, 2) *= scale;
      K(1, 2) *= scale;
    }

    cv::Mat1d P;
    if (!P_.empty()) {
      P = P_.clone();
      P(0, 0) *= scale;
      P(1, 1) *= scale;
      P(0, 2) *= scale;
      P(1, 2) *= scale;
      P(0, 3) *= scale;
      P(1, 3) *= scale;
    }
    scaled_model =
        CameraModel(name_,
                    cv::Size(static_cast<double>(image_size_.width) * scale,
                             static_cast<double>(image_size_.height) * scale),
                    K, D_.matrix(), R_, P);
  } else {
    LOG(WARNING)
        << "Trying to scale a camera model not valid! Ignoring scaling...";
  }
  return scaled_model;
}

CameraModel CameraModel::Roi(const cv::Rect& roi) const {
  CameraModel roi_model = *this;
  if (IsValidForProjection()) {
    // has only effect on cx and cy
    cv::Mat1d K;
    if (!K_.matrix().empty()) {
      K = K_.matrix().clone();
      K(0, 2) -= roi.x;
      K(1, 2) -= roi.y;
    }

    cv::Mat1d P;
    if (!P_.empty()) {
      P = P_.clone();
      P(0, 2) -= roi.x;
      P(1, 2) -= roi.y;
    }
    roi_model = CameraModel(name_, roi.size(), K, D_.matrix(), R_, P);
  } else {
    LOG(WARNING) << "Trying to extract roi from a camera model not valid! "
                    "Ignoring roi...";
  }
  return roi_model;
}

double CameraModel::HorizontalFOV() const {
  if (image_width() > 0 && fx() > 0.0) {
    return atan((static_cast<double>(image_width()) / 2.0) / fx()) * 2.0;
  }
  return 0.0;
}

double CameraModel::VerticalFOV() const {
  if (image_height() > 0 && fy() > 0.0) {
    return atan((static_cast<double>(image_height()) / 2.0) / fy()) * 2.0;
  }
  return 0.0;
}

cv::Mat CameraModel::RectifyImage(const cv::Mat& raw, int interpolation) const {
  if (IsRectificationMapInitialized()) {
    cv::Mat rectified;
    cv::remap(raw, rectified, map_x_, map_y_, interpolation);
    return rectified;
  } else {
    LOG(ERROR)
        << "Cannot rectify image because the rectify map is not initialized.";
    return raw.clone();
  }
}

// inspired from
// https://github.com/code-iai/iai_kinect2/blob/master/depth_registration/src/depth_registration_cpu.cpp
cv::Mat1u CameraModel::RectifyDepth(const cv::Mat1u& raw) const {
  if (IsRectificationMapInitialized()) {
    cv::Mat1u rectified = cv::Mat1u::zeros(map_x_.rows, map_x_.cols);
    for (int y = 0; y < map_x_.rows; ++y) {
      for (int x = 0; x < map_x_.cols; ++x) {
        cv::Point2f pt(map_x_.at<float>(y, x), map_y_.at<float>(y, x));
        int xL = static_cast<int>(floor(pt.x));
        int xH = static_cast<int>(ceil(pt.x));
        int yL = static_cast<int>(floor(pt.y));
        int yH = static_cast<int>(ceil(pt.y));
        if (xL >= 0 && yL >= 0 && xH < raw.cols && yH < raw.rows) {
          const uint16_t& pLT = raw(yL, xL);
          const uint16_t& pRT = raw(yL, xH);
          const uint16_t& pLB = raw(yH, xL);
          const uint16_t& pRB = raw(yH, xH);
          if (pLT > 0 && pRT > 0 && pLB > 0 && pRB > 0) {
            uint16_t avg = (pLT + pRT + pLB + pRB) / 4;
            uint16_t thres = 0.01 * avg;
            if (abs(pLT - avg) < thres && abs(pRT - avg) < thres &&
                abs(pLB - avg) < thres && abs(pRB - avg) < thres) {
              // bilinear interpolation
              float a = pt.x - static_cast<float>(xL);
              float c = pt.y - static_cast<float>(yL);

              // http://stackoverflow.com/questions/13299409/how-to-get-the-image-pixel-at-real-locations-in-opencv
              rectified(y, x) = (pLT * (1.f - a) + pRT * a) * (1.f - c) +
                                (pLB * (1.f - a) + pRB * a) * c;
            }
          }
        }
      }
    }
    return rectified;
  } else {
    LOG(ERROR)
        << "Cannot rectify image because the rectify map is not initialized.";
    return raw.clone();
  }
}

bool CameraModel::InImage(int u, int v) const {
  return u >= 0 && u < image_width() && v >= 0 && v < image_height();
}

}  // namespace slam
}  // namespace felicia
