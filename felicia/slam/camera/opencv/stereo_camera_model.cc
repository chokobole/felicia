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

#include "felicia/slam/camera/opencv/stereo_camera_model.h"

#include <opencv2/imgproc/imgproc.hpp>

#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/math/matrix_util.h"

namespace felicia {
namespace slam {

namespace {

bool IsValidRMatrix(const cv::Mat1d& R) {
  return R.empty() || IsMatrix<3, 3>(R.rows, R.cols);
}

bool IsValidTMatrix(const cv::Mat1d& T) {
  return T.empty() || IsMatrix<3, 1>(T.rows, T.cols);
}

bool IsValidEMatrix(const cv::Mat1d& E) {
  return E.empty() || IsMatrix<3, 3>(E.rows, E.cols);
}

bool IsValidFMatrix(const cv::Mat1d& F) {
  return F.empty() || IsMatrix<3, 3>(F.rows, F.cols);
}

}  // namespace

StereoCameraModel::StereoCameraModel() = default;

StereoCameraModel::StereoCameraModel(
    const std::string& name, const std::string& name1,
    const cv::Size& image_size1, const cv::Mat1d& K1, const cv::Mat1d& D1,
    const cv::Mat1d& R1, const cv::Mat1d& P1, const std::string& name2,
    const cv::Size& image_size2, const cv::Mat1d& K2, const cv::Mat1d& D2,
    const cv::Mat1d& R2, const cv::Mat1d& P2, const cv::Mat1d& R,
    const cv::Mat1d& T, const cv::Mat1d& E, const cv::Mat1d& F)
    : StereoCameraModel(name, CameraModel{name1, image_size1, K1, D1, R1, P1},
                        CameraModel{name2, image_size2, K2, D2, R2, P2}, R, T,
                        E, F) {}

StereoCameraModel::StereoCameraModel(const std::string& name,
                                     const CameraModel& left_camera_model,
                                     const CameraModel& right_camera_model,
                                     const cv::Mat1d& R, const cv::Mat1d& T,
                                     const cv::Mat1d& E, const cv::Mat1d& F)
    : name_(name),
      left_camera_model_(left_camera_model),
      right_camera_model_(right_camera_model),
      transform_(R, T),
      E_(E),
      F_(F) {
  CHECK(IsValidRMatrix(R));
  CHECK(IsValidTMatrix(T));
  CHECK(IsValidEMatrix(E));
  CHECK(IsValidFMatrix(F));
  RectifyStereo();
}

StereoCameraModel::StereoCameraModel(double fx, double fy, double cx, double cy,
                                     double baseline,
                                     const cv::Size& image_size)
    : StereoCameraModel(base::EmptyString(), base::EmptyString(),
                        base::EmptyString(), fx, fy, cx, cy, baseline,
                        image_size) {}

StereoCameraModel::StereoCameraModel(const std::string& name,
                                     const std::string& left_camera_model_name,
                                     const std::string& right_camera_model_name,
                                     double fx, double fy, double cx, double cy,
                                     double baseline,
                                     const cv::Size& image_size)
    : name_(name),
      left_camera_model_(left_camera_model_name, fx, fy, cx, cy, 0, image_size),
      right_camera_model_(right_camera_model_name, fx, fy, cx, cy,
                          baseline * -fx, image_size)

{}

Status StereoCameraModel::Load(const base::FilePath& left_camera_model_path,
                               const base::FilePath& right_camera_model_path,
                               const base::FilePath& stereo_transform_path) {
  Status s = left_camera_model_.Load(left_camera_model_path);
  if (!s.ok()) return s;

  s = right_camera_model_.Load(right_camera_model_path);
  if (!s.ok()) return s;

  if (stereo_transform_path.empty()) return Status::OK();

  return LoadStereoTransform(stereo_transform_path);
}

Status StereoCameraModel::Save(
    const base::FilePath& left_camera_model_path,
    const base::FilePath& right_camera_model_path,
    const base::FilePath& stereo_transform_path) const {
  Status s = left_camera_model_.Save(left_camera_model_path);
  if (!s.ok()) return s;

  s = right_camera_model_.Save(right_camera_model_path);
  if (!s.ok()) return s;

  if (stereo_transform_path.empty()) return Status::OK();

  return SaveStereoTransform(stereo_transform_path);
}

Status StereoCameraModel::LoadStereoTransform(const base::FilePath& path) {
  try {
    cv::FileStorage fs(path.AsUTF8Unsafe(), cv::FileStorage::READ);

    std::string name = name_;
    cv::Mat1d R = transform_.R();
    cv::Mat1d T = transform_.t();
    cv::Mat1d E = E_;
    cv::Mat1d F = F_;

    Status s;

    internal::MaybeLoad(fs, "camera_name", path,
                        [&name](const cv::FileNode& n) {
                          name = static_cast<std::string>(n);
                          return Status::OK();
                        });

    // import from ROS calibration format
    s = internal::MaybeLoad(fs, "rotation_matrix", path,
                            [&R](const cv::FileNode& n) {
                              StatusOr<cv::Mat1d> status_or =
                                  internal::LoadCvMatrix(n, IsMatrix<3, 3>);
                              if (!status_or.ok()) return status_or.status();
                              R = status_or.ValueOrDie();
                              return Status::OK();
                            });
    if (!s.ok()) return s;

    s = internal::MaybeLoad(fs, "translation_matrix", path,
                            [&T](const cv::FileNode& n) {
                              StatusOr<cv::Mat1d> status_or =
                                  internal::LoadCvMatrix(n, IsMatrix<3, 1>);
                              if (!status_or.ok()) return status_or.status();
                              T = status_or.ValueOrDie();
                              return Status::OK();
                            });
    if (!s.ok()) return s;

    s = internal::MaybeLoad(fs, "essential_matrix", path,
                            [&E](const cv::FileNode& n) {
                              StatusOr<cv::Mat1d> status_or =
                                  internal::LoadCvMatrix(n, IsMatrix<3, 3>);
                              if (!status_or.ok()) return status_or.status();
                              E = status_or.ValueOrDie();
                              return Status::OK();
                            });
    if (!s.ok()) return s;

    s = internal::MaybeLoad(fs, "fundamental_matrix", path,
                            [&F](const cv::FileNode& n) {
                              StatusOr<cv::Mat1d> status_or =
                                  internal::LoadCvMatrix(n, IsMatrix<3, 3>);
                              if (!status_or.ok()) return status_or.status();
                              F = status_or.ValueOrDie();
                              return Status::OK();
                            });
    if (!s.ok()) return s;

    fs.release();

    name_ = name;
    transform_.R() = R;
    transform_.t() = T;
    E_ = E;
    F_ = F;
  } catch (const cv::Exception& e) {
    return errors::InvalidArgument(
        base::StringPrintf("Error reading stereo extrinsics file %s : %s",
                           path.value().c_str(), e.what()));
  }

  return Status::OK();
}

Status StereoCameraModel::SaveStereoTransform(
    const base::FilePath& path) const {
  const cv::Mat1d& R = transform_.R();
  const cv::Mat1d& T = transform_.t();
  if (!R.empty() && !T.empty()) {
    try {
      cv::FileStorage fs(path.AsUTF8Unsafe(), cv::FileStorage::WRITE);

      // export in ROS calibration format
      if (!name_.empty()) {
        fs << "camera_name" << name_;
      }

      if (!R.empty()) {
        fs << "rotation_matrix"
           << "{";
        fs << "rows" << R.rows;
        fs << "cols" << R.cols;
        fs << "data"
           << std::vector<double>(
                  reinterpret_cast<double*>(R.data),
                  reinterpret_cast<double*>(R.data) + (R.rows * R.cols));
        fs << "}";
      }

      if (!T.empty()) {
        fs << "translation_matrix"
           << "{";
        fs << "rows" << T.rows;
        fs << "cols" << T.cols;
        fs << "data"
           << std::vector<double>(
                  reinterpret_cast<double*>(T.data),
                  reinterpret_cast<double*>(T.data) + (T.rows * T.cols));
        fs << "}";
      }

      if (!E_.empty()) {
        fs << "essential_matrix"
           << "{";
        fs << "rows" << E_.rows;
        fs << "cols" << E_.cols;
        fs << "data"
           << std::vector<double>(
                  reinterpret_cast<double*>(E_.data),
                  reinterpret_cast<double*>(E_.data) + (E_.rows * E_.cols));
        fs << "}";
      }

      if (!F_.empty()) {
        fs << "fundamental_matrix"
           << "{";
        fs << "rows" << F_.rows;
        fs << "cols" << F_.cols;
        fs << "data"
           << std::vector<double>(
                  reinterpret_cast<double*>(F_.data),
                  reinterpret_cast<double*>(F_.data) + (F_.rows * F_.cols));
        fs << "}";
      }

      fs.release();
    } catch (const cv::Exception& e) {
      return errors::InvalidArgument(
          base::StringPrintf("Error writing stereo extrnisics file %s : %s",
                             path.value().c_str(), e.what()));
    }
  } else {
    return errors::InvalidArgument(
        "Cannot save stereo extrinsics, check if R and T are not empty");
  }
  return Status::OK();
}

StereoCameraModelMessage StereoCameraModel::ToStereoCameraModelMessage() const {
  StereoCameraModelMessage message;
  *message.mutable_left_camera_model() =
      left_camera_model_.ToCameraModelMessage();
  *message.mutable_right_camera_model() =
      right_camera_model_.ToCameraModelMessage();
  const cv::Mat1d& R = transform_.R();
  const cv::Mat1d& T = transform_.t();
  if (!R.empty()) message.set_r(R.data, R.total() * sizeof(double));
  if (!T.empty()) message.set_t(T.data, T.total() * sizeof(double));
  if (!E_.empty()) message.set_e(E_.data, E_.total() * sizeof(double));
  if (!F_.empty()) message.set_f(F_.data, F_.total() * sizeof(double));

  return message;
}

Status StereoCameraModel::FromStereoCameraModelMessage(
    const StereoCameraModelMessage& message) {
  CameraModel left_camera_model = left_camera_model_;
  CameraModel right_camera_model = right_camera_model_;
  cv::Mat1d R = transform_.R();
  cv::Mat1d T = transform_.t();
  cv::Mat1d E = E_;
  cv::Mat1d F = F_;

  if (message.has_left_camera_model()) {
    Status s =
        left_camera_model_.FromCameraModelMessage(message.left_camera_model());
    if (!s.ok()) return s;
  }

  if (message.has_right_camera_model()) {
    Status s = right_camera_model_.FromCameraModelMessage(
        message.right_camera_model());
    if (!s.ok()) return s;
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

  if (!message.t().empty()) {
    const std::string& t = message.t();
    int rows = 3;
    int cols = t.length() / rows;
    if (!IsMatrix<3, 1>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    T = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(t.c_str())))
            .clone();
  }

  if (!message.e().empty()) {
    const std::string& e = message.e();
    int rows = 3;
    int cols = e.length() / rows;
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    E = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(e.c_str())))
            .clone();
  }

  if (!message.f().empty()) {
    const std::string& f = message.f();
    int rows = 3;
    int cols = f.length() / rows;
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    F = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(f.c_str())))
            .clone();
  }

  left_camera_model_ = left_camera_model;
  right_camera_model_ = right_camera_model;
  transform_.R() = R;
  transform_.t() = T;
  E_ = E;
  F_ = F;

  return Status::OK();
}

Status StereoCameraModel::FromStereoCameraModelMessage(
    StereoCameraModelMessage&& message) {
  CameraModel left_camera_model = left_camera_model_;
  CameraModel right_camera_model = right_camera_model_;
  cv::Mat1d R = transform_.R();
  cv::Mat1d T = transform_.t();
  cv::Mat1d E = E_;
  cv::Mat1d F = F_;

  if (message.has_left_camera_model()) {
    Status s = left_camera_model_.FromCameraModelMessage(
        std::move(message.left_camera_model()));
    if (!s.ok()) return s;
  }

  if (message.has_right_camera_model()) {
    Status s = right_camera_model_.FromCameraModelMessage(
        std::move(message.right_camera_model()));
    if (!s.ok()) return s;
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

  if (!message.t().empty()) {
    std::string* t = message.release_t();
    int rows = 3;
    int cols = t->length() / rows;
    if (!IsMatrix<3, 1>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    T = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(t->c_str())));
  }

  if (!message.e().empty()) {
    std::string* e = message.release_e();
    int rows = 3;
    int cols = e->length() / rows;
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    E = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(e->c_str())));
  }

  if (!message.f().empty()) {
    std::string* f = message.release_f();
    int rows = 3;
    int cols = f->length() / rows;
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    F = cv::Mat1d(rows, cols,
                  reinterpret_cast<double*>(const_cast<char*>(f->c_str())));
  }

  left_camera_model_ = left_camera_model;
  right_camera_model_ = right_camera_model;
  transform_.R() = R;
  transform_.t() = T;
  E_ = E;
  F_ = F;

  return Status::OK();
}

float StereoCameraModel::ComputeDepth(float disparity) const {
  // depth = baseline * f / (disparity + cx1-cx0);
  CHECK(IsValidForProjection());
  if (disparity == 0.0f) {
    return 0.0f;
  }
  return baseline() * left_camera_model().fx() /
         (disparity + right_camera_model().cx() - left_camera_model().cx());
}

float StereoCameraModel::ComputeDisparity(float depth) const {
  // disparity = (baseline * fx / depth) - (cx1-cx0);
  CHECK(IsValidForProjection());
  if (depth == 0.0f) {
    return 0.0f;
  }
  return baseline() * left_camera_model().fx() / depth -
         right_camera_model().cx() + left_camera_model().cx();
}

void StereoCameraModel::RectifyStereo() {
  const cv::Mat1d& R = transform_.R();
  const cv::Mat1d& T = transform_.t();
  if (!R.empty() && !T.empty()) {
    CHECK(IsValidForRectification());

    cv::Mat1d R1, R2, P1, P2, Q;
    cv::stereoRectify(left_camera_model_.K_raw(), left_camera_model_.D_raw(),
                      right_camera_model_.K_raw(), right_camera_model_.D_raw(),
                      left_camera_model_.image_size(), R, T, R1, R2, P1, P2, Q,
                      cv::CALIB_ZERO_DISPARITY, 0,
                      left_camera_model_.image_size());

    left_camera_model_ = CameraModel(
        left_camera_model_.name(), left_camera_model_.image_size(),
        left_camera_model_.K_raw(), left_camera_model_.D_raw(), R1, P1);
    right_camera_model_ = CameraModel(
        right_camera_model_.name(), right_camera_model_.image_size(),
        right_camera_model_.K_raw(), right_camera_model_.D_raw(), R2, P2);
  }
}

}  // namespace slam
}  // namespace felicia
