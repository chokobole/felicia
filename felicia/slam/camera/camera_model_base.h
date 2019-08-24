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

#ifndef FELICIA_SLAM_CAMERA_CAMERA_MODEL_BASE_H_
#define FELICIA_SLAM_CAMERA_CAMERA_MODEL_BASE_H_

#include "third_party/chromium/base/files/file_path.h"
#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/lib/file/yaml_reader.h"
#include "felicia/core/lib/file/yaml_writer.h"
#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/core/lib/unit/geometry/rect.h"
#include "felicia/core/lib/unit/geometry/size.h"
#include "felicia/slam/camera/camera_matrix.h"
#include "felicia/slam/camera/camera_model_message.pb.h"
#include "felicia/slam/camera/distortion_matrix.h"
#include "felicia/slam/camera/projection_matrix.h"
#include "felicia/slam/camera/rectification_matrix.h"

namespace felicia {
namespace slam {

namespace internal {

Status InvalidRowsAndCols(int rows, int cols);
Status InvalidImageSize(int width, int height);
bool IsValidImageSize(int width, int height);

Status MaybeLoad(const YAML::Node& node, const std::string& name,
                 const base::FilePath& path,
                 std::function<Status(const YAML::Node&)> callback);

template <typename T, typename MatrixType = typename T::MatrixType,
          typename ScalarType = typename T::ScalarType>
StatusOr<MatrixType> LoadMatrix(const YAML::Node& n,
                                std::function<bool(int, int)> callback) {
  int rows = n["rows"].as<int>();
  int cols = n["cols"].as<int>();
  if (!callback(rows, cols)) {
    return internal::InvalidRowsAndCols(rows, cols);
  }
  std::vector<ScalarType> data = n["data"].as<std::vector<ScalarType>>();
  if (rows * cols != static_cast<int>(data.size())) {
    return errors::InvalidArgument(base::StringPrintf(
        "rowsxcols are not matched with data size. %dx%d = %d", rows, cols,
        static_cast<int>(data.size())));
  }
  return ConstNativeMatrixRef<MatrixType>::NewMatrix(data.data(), rows, cols);
}

}  // namespace internal

template <typename KType, typename DType, typename RType, typename PType>
class CameraModelBase {
 public:
  typedef typename KType::MatrixType KMatrixType;
  typedef typename DType::MatrixType DMatrixType;
  typedef typename RType::MatrixType RMatrixType;
  typedef typename PType::MatrixType PMatrixType;
  typedef typename KType::ScalarType ScalarType;
  static_assert(std::is_same<typename KType::ScalarType,
                             typename DType::ScalarType>::value &&
                    std::is_same<typename DType::ScalarType,
                                 typename RType::ScalarType>::value &&
                    std::is_same<typename RType::ScalarType,
                                 typename PType::ScalarType>::value,
                "All ScalarTypes should be same");

  CameraModelBase() = default;

  CameraModelBase(const std::string& name, const Sizei& image_size,
                  const KType& K, const DType& D, const RType& R,
                  const PType& P)
      : name_(name), image_size_(image_size), K_(K), D_(D), R_(R), P_(P) {}

  CameraModelBase(const std::string& name, const Sizei& image_size,
                  const KMatrixType& K, const DMatrixType& D,
                  const RMatrixType& R, const PMatrixType& P)
      : CameraModelBase(name, image_size, KType{K}, DType{D}, RType{R},
                        PType{P}) {}

  // minimal
  CameraModelBase(ScalarType fx, ScalarType fy, ScalarType cx, ScalarType cy,
                  ScalarType tx = 0.0, const Sizei& image_size = Sizei(0, 0))
      : CameraModelBase(base::EmptyString(), fx, fy, cx, cy, tx, image_size) {}
  // minimal to be saved
  CameraModelBase(const std::string& name, ScalarType fx, ScalarType fy,
                  ScalarType cx, ScalarType cy, ScalarType tx = 0.0,
                  const Sizei image_size = Sizei(0, 0));

  void set_name(const std::string& name) { name_ = name; }
  const std::string& name() const { return name_; }

  ScalarType fx() const {
    ConstNativeMatrixRef<PMatrixType> P_ref(P_.matrix());
    return P_ref.empty() ? K_.fx() : P_.fx();
  }
  ScalarType fy() const {
    ConstNativeMatrixRef<PMatrixType> P_ref(P_.matrix());
    return P_ref.empty() ? K_.fy() : P_.fy();
  }
  ScalarType cx() const {
    ConstNativeMatrixRef<PMatrixType> P_ref(P_.matrix());
    return P_ref.empty() ? K_.cx() : P_.cx();
  }
  ScalarType cy() const {
    ConstNativeMatrixRef<PMatrixType> P_ref(P_.matrix());
    return P_ref.empty() ? K_.cy() : P_.cy();
  }
  ScalarType tx() const {
    ConstNativeMatrixRef<PMatrixType> P_ref(P_.matrix());
    return P_ref.empty() ? 0.0 : P_.tx();
  }

  bool IsValidForProjection() const {
    return fx() > 0.0 && fy() > 0.0 && cx() > 0.0 && cy() > 0.0;
  }
  bool IsValidForRectification() const {
    ConstNativeMatrixRef<PMatrixType> K_ref(K_.matrix());
    ConstNativeMatrixRef<PMatrixType> D_ref(D_.matrix());
    ConstNativeMatrixRef<PMatrixType> R_ref(R_.matrix());
    ConstNativeMatrixRef<PMatrixType> P_ref(P_.matrix());
    return image_size_.width() > 0 && image_size_.height() > 0 &&
           !K_ref.empty() && !D_ref.empty() && !R_ref.empty() && !P_ref.empty();
  }

  // intrinsic camera matrix (before rectification)
  const KType& K_raw() const { return K_; }
  // intrinsic distorsion matrix (before rectification)
  const DType& D_raw() const { return D_; }
  // if P exists, return rectified version
  KType K() const {
    ConstNativeMatrixRef<PMatrixType> P_ref(P_.matrix());
    if (P_ref.empty()) return K_;
    return KType{P_ref.block(0, 0, 3, 3)};
  }
  // if P exists, return rectified version
  DType D() const {
    ConstNativeMatrixRef<PMatrixType> P_ref(P_.matrix());
    ConstNativeMatrixRef<PMatrixType> D_ref(D_.matrix());
    if (P_ref.empty() && !D_ref.empty()) return D_;
    return {};
  }
  // rectification matrix
  const RType& R() const { return R_; }
  // projection matrix
  const PType& P() const { return P_; }

  void set_image_size(const Sizei& size) { image_size_ = size; }
  const Sizei& image_size() const { return image_size_; }
  int image_width() const { return image_size_.width(); }
  int image_height() const { return image_size_.height(); }

  Status Load(const base::FilePath& path);
  Status Save(const base::FilePath& path) const;

  CameraModelMessage ToCameraModelMessage() const;
  Status FromCameraModelMessage(const CameraModelMessage& message);
  Status FromCameraModelMessage(CameraModelMessage&& message);

  CameraModelBase Scaled(double scale) const;
  CameraModelBase Roi(const Recti& roi) const;

  double HorizontalFOV() const;  // in radians
  double VerticalFOV() const;    // in radians

  template <typename T>
  Point<T> Project(const Point3<T>& point) const {
    if (P_.matrix().empty()) return P_.Project(point);
    return K_.Project(point);
  }

  template <typename T>
  Point3<T> ProjectInverse(const Point<T>& point, T depth) const {
    if (P_.matrix().empty()) return P_.ProjectInverse(point, depth);
    return K_.ProjectInverse(point);
  }

  bool InImage(int u, int v) const {
    return u >= 0 && u < image_width() && v >= 0 && v < image_height();
  }

 protected:
  std::string name_;
  Sizei image_size_;
  // The camera intrinsic 3x3
  KType K_;
  // The distortion coefficients 1x4,5,6,8
  DType D_;
  // The rectification matrix 3x3 (computed from stereo or Identity)
  RType R_;
  // The projection matrix 3x4 (computed from stereo or equal to [K [0
  // 0 1]'])
  PType P_;
};

template <typename KType, typename DType, typename RType, typename PType>
CameraModelBase<KType, DType, RType, PType>::CameraModelBase(
    const std::string& name, ScalarType fx, ScalarType fy, ScalarType cx,
    ScalarType cy, ScalarType tx, const Sizei image_size)
    : name_(name), image_size_(image_size), K_(fx, fy, cx, cy) {
  if (tx != 0.0) {
    P_ = PType{fx, fy, cx, cy, tx};
  }

  if (cx == 0.0 && image_size_.width() > 0) {
    double ncx = static_cast<double>(image_size_.width) / 2.0 - 0.5;
    if (tx != 0.0) {
      P_.set_cx(ncx);
    }
    K_.set_cx(ncx);
  }

  if (cy == 0.0 && image_size_.height() > 0) {
    double ncy = static_cast<double>(image_size_.height) / 2.0 - 0.5;
    if (tx != 0.0) {
      P_.set_cy(ncy);
    }
    K_.set_cy(ncy);
  }
}

template <typename KType, typename DType, typename RType, typename PType>
Status CameraModelBase<KType, DType, RType, PType>::Load(
    const base::FilePath& path) {
  YamlReader reader;
  Status s = reader.Open(path);
  if (!s.ok()) return s;
  const YAML::Node& node = reader.node();

  std::string name;
  Sizei image_size;
  KType K;
  DType D;
  RType R;
  PType P;

  internal::MaybeLoad(node, "camera_name", path, [&name](const YAML::Node& n) {
    name = n.as<std::string>();
    return Status::OK();
  });
  internal::MaybeLoad(node, "image_width", path,
                      [&image_size](const YAML::Node& n) {
                        image_size.set_width(n.as<int>());
                        return Status::OK();
                      });
  internal::MaybeLoad(node, "image_height", path,
                      [&image_size](const YAML::Node& n) {
                        image_size.set_height(n.as<int>());
                        return Status::OK();
                      });
  if (!internal::IsValidImageSize(image_size.width(), image_size.height()))
    return internal::InvalidImageSize(image_size.width(), image_size.height());

  // import from ROS calibration format
  s = internal::MaybeLoad(node, "camera_matrix", path,
                          [&K](const YAML::Node& n) {
                            StatusOr<KMatrixType> status_or =
                                internal::LoadMatrix<KType>(n, IsMatrix<3, 3>);
                            if (!status_or.ok()) return status_or.status();
                            K = status_or.ValueOrDie();
                            return Status::OK();
                          });
  if (!s.ok()) return s;

  DistortionModel distortion_model;
  internal::MaybeLoad(
      node, "distortion_model", path, [&distortion_model](const YAML::Node& n) {
        distortion_model = ToDistortionModel(n.as<std::string>());
        return Status::OK();
      });

  s = internal::MaybeLoad(
      node, "distortion_coefficients", path,
      [distortion_model, &D](const YAML::Node& n) {
        StatusOr<DMatrixType> status_or =
            internal::LoadMatrix<DType>(n, IsValidDistortionMatrix);
        if (!status_or.ok()) return status_or.status();
        D = status_or.ValueOrDie();
        if (D.distortion_model() != distortion_model) {
          CHECK(distortion_model == DISTORTION_MODEL_EQUIDISTANT &&
                D.distortion_model() == DISTORTION_MODEL_PLUMB_BOB);
          D = DType(D.k1(), D.k2(), D.p1(), D.p2(), distortion_model);
        }
        return Status::OK();
      });
  if (!s.ok()) return s;

  s = internal::MaybeLoad(node, "rectification_matrix", path,
                          [&R](const YAML::Node& n) {
                            StatusOr<RMatrixType> status_or =
                                internal::LoadMatrix<RType>(n, IsMatrix<3, 3>);
                            if (!status_or.ok()) return status_or.status();
                            R = status_or.ValueOrDie();
                            return Status::OK();
                          });
  if (!s.ok()) return s;

  s = internal::MaybeLoad(node, "projection_matrix", path,
                          [&P](const YAML::Node& n) {
                            StatusOr<PMatrixType> status_or =
                                internal::LoadMatrix<PType>(n, IsMatrix<3, 4>);
                            if (!status_or.ok()) return status_or.status();
                            P = status_or.ValueOrDie();
                            return Status::OK();
                          });
  if (!s.ok()) return s;

  name_ = name;
  image_size_ = image_size;
  K_ = K;
  D_ = D;
  R_ = R;
  P_ = P;

  return Status::OK();
}

template <typename KType, typename DType, typename RType, typename PType>
Status CameraModelBase<KType, DType, RType, PType>::Save(
    const base::FilePath& path) const {
  ConstNativeMatrixRef<KMatrixType> K_ref(K_.matrix());
  ConstNativeMatrixRef<DMatrixType> D_ref(D_.matrix());
  ConstNativeMatrixRef<RMatrixType> R_ref(R_.matrix());
  ConstNativeMatrixRef<PMatrixType> P_ref(P_.matrix());
  if (!K_ref.empty() || !D_ref.empty() || !R_ref.empty() || !P_ref.empty()) {
    YamlWriter yaml_writer;
    YAML::Emitter& emitter = yaml_writer.emitter();

    // export in ROS calibration format
    emitter << YAML::BeginMap;
    if (!name_.empty()) {
      emitter << YAML::Key << "camera_name" << YAML::Value << name_;
    }

    if (image_size_.width() > 0 && image_size_.height() > 0) {
      emitter << YAML::Key << "image_width" << YAML::Value
              << image_size_.width();
      emitter << YAML::Key << "image_height" << YAML::Value
              << image_size_.height();
    }

    if (!K_ref.empty()) {
      emitter << YAML::Key << "camera_matrix";
      emitter << YAML::BeginMap;
      emitter << YAML::Key << "rows" << YAML::Value << K_ref.rows();
      emitter << YAML::Key << "cols" << YAML::Value << K_ref.cols();
      emitter << YAML::Key << "data" << YAML::Value << YAML::Flow
              << std::vector<ScalarType>(
                     reinterpret_cast<const ScalarType*>(K_ref.data()),
                     reinterpret_cast<const ScalarType*>(K_ref.data()) +
                         K_ref.size());
      emitter << YAML::EndMap;
    }

    if (!D_ref.empty()) {
      DistortionModel distortion_model = D_.distortion_model();
      std::vector<ScalarType> params;
      if (distortion_model == DISTORTION_MODEL_EQUIDISTANT) {
        params = std::vector<ScalarType>{D_.k1(), D_.k2(), D_.k3(), D_.k4()};
      } else {
        params = std::vector<ScalarType>(
            reinterpret_cast<const ScalarType*>(D_ref.data()),
            reinterpret_cast<const ScalarType*>(D_ref.data()) + D_ref.size());
      }
      emitter << YAML::Key << "distortion_coefficients";
      emitter << YAML::BeginMap;
      emitter << YAML::Key << "rows" << YAML::Value << 1;
      emitter << YAML::Key << "cols" << YAML::Value << params.size();
      emitter << YAML::Key << "data" << YAML::Value << YAML::Flow << params;
      emitter << YAML::EndMap;

      // compaibility with ROS
      emitter << YAML::Key << "distortion_model" << YAML::Value
              << DistortionModelToString(D_.distortion_model());
    }

    if (!R_ref.empty()) {
      emitter << YAML::Key << "rectification_matrix";
      emitter << YAML::BeginMap;
      emitter << YAML::Key << "rows" << YAML::Value << R_ref.rows();
      emitter << YAML::Key << "cols" << YAML::Value << R_ref.cols();
      emitter << YAML::Key << "data" << YAML::Value << YAML::Flow
              << std::vector<ScalarType>(
                     reinterpret_cast<const ScalarType*>(R_ref.data()),
                     reinterpret_cast<const ScalarType*>(R_ref.data()) +
                         R_ref.size());
      emitter << YAML::EndMap;
    }

    if (!P_ref.empty()) {
      emitter << YAML::Key << "projection_matrix";
      emitter << YAML::BeginMap;
      emitter << YAML::Key << "rows" << YAML::Value << P_ref.rows();
      emitter << YAML::Key << "cols" << YAML::Value << P_ref.cols();
      emitter << YAML::Key << "data" << YAML::Value << YAML::Flow
              << std::vector<ScalarType>(
                     reinterpret_cast<const ScalarType*>(P_ref.data()),
                     reinterpret_cast<const ScalarType*>(P_ref.data()) +
                         P_ref.size());
      emitter << YAML::EndMap;
    }

    Status s = yaml_writer.WriteToFile(path);
    if (!s.ok()) return s;
  } else {
    return errors::InvalidArgument(
        "Cannot save calibration, check if K, D, R and P are not empty");
  }
  return Status::OK();
}  // namespace slam

template <typename KType, typename DType, typename RType, typename PType>
CameraModelMessage
CameraModelBase<KType, DType, RType, PType>::ToCameraModelMessage() const {
  CameraModelMessage message;
  *message.mutable_image_size() = SizeiToSizeiMessage(image_size_);
  ConstNativeMatrixRef<KMatrixType> K_ref(K_.matrix());
  ConstNativeMatrixRef<DMatrixType> D_ref(D_.matrix());
  ConstNativeMatrixRef<RMatrixType> R_ref(R_.matrix());
  ConstNativeMatrixRef<PMatrixType> P_ref(P_.matrix());
  if (!K_ref.empty()) message.set_k(K_ref.data(), K_ref.allocation_size());
  if (!D_ref.empty()) message.set_d(D_ref.data(), D_ref.allocation_size());
  if (!R_ref.empty()) message.set_r(R_ref.data(), R_ref.allocation_size());
  if (!P_ref.empty()) message.set_p(P_ref.data(), P_ref.allocation_size());

  return message;
}

template <typename KType, typename DType, typename RType, typename PType>
Status CameraModelBase<KType, DType, RType, PType>::FromCameraModelMessage(
    const CameraModelMessage& message) {
  Sizei image_size;
  KType K;
  DType D;
  RType R;
  PType P;

  if (message.has_image_size()) {
    int width = message.image_size().width();
    int height = message.image_size().height();
    if (!internal::IsValidImageSize(width, height))
      return internal::InvalidImageSize(width, height);
    image_size.set_width(width);
    image_size.set_height(height);
  }

  if (!message.k().empty()) {
    const std::string& k = message.k();
    int rows = 3;
    int cols = k.length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    K = ConstNativeMatrixRef<KMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(k.c_str())), rows,
        cols);
  }

  if (!message.d().empty()) {
    const std::string& d = message.d();
    int rows = 1;
    int cols = d.length() / (rows * sizeof(ScalarType));
    if (!IsValidDistortionMatrix(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    D = ConstNativeMatrixRef<DMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(d.c_str())), rows,
        cols);
  }

  if (!message.r().empty()) {
    const std::string& r = message.r();
    int rows = 3;
    int cols = r.length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    R = ConstNativeMatrixRef<RMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(r.c_str())), rows,
        cols);
  }

  if (!message.p().empty()) {
    const std::string& p = message.p();
    int rows = 3;
    int cols = p.length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 4>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    P = ConstNativeMatrixRef<PMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(p.c_str())), rows,
        cols);
  }

  image_size_ = image_size;
  K_ = K;
  D_ = D;
  R_ = R;
  P_ = P;

  return Status::OK();
}

template <typename KType, typename DType, typename RType, typename PType>
Status CameraModelBase<KType, DType, RType, PType>::FromCameraModelMessage(
    CameraModelMessage&& message) {
  Sizei image_size;
  KType K;
  DType D;
  RType R;
  PType P;

  if (message.has_image_size()) {
    int width = message.image_size().width();
    int height = message.image_size().height();
    if (!internal::IsValidImageSize(width, height))
      return internal::InvalidImageSize(width, height);
    image_size.set_width(width);
    image_size.set_height(height);
  }

  if (!message.k().empty()) {
    std::unique_ptr<std::string> k(message.release_k());
    int rows = 3;
    int cols = k->length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    K = ConstNativeMatrixRef<KMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(k->c_str())), rows,
        cols);
  }

  if (!message.d().empty()) {
    std::unique_ptr<std::string> d(message.release_d());
    int rows = 1;
    int cols = d->length() / (rows * sizeof(ScalarType));
    if (!IsValidDistortionMatrix(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    D = ConstNativeMatrixRef<DMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(d->c_str())), rows,
        cols);
  }

  if (!message.r().empty()) {
    std::unique_ptr<std::string> r(message.release_r());
    int rows = 3;
    int cols = r->length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    R = ConstNativeMatrixRef<RMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(r->c_str())), rows,
        cols);
  }

  if (!message.p().empty()) {
    std::unique_ptr<std::string> p(message.release_p());
    int rows = 3;
    int cols = p->length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 4>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    P = ConstNativeMatrixRef<PMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(p->c_str())), rows,
        cols);
  }

  image_size_ = image_size;
  K_ = K;
  D_ = D;
  R_ = R;
  P_ = P;

  return Status::OK();
}

template <typename KType, typename DType, typename RType, typename PType>
CameraModelBase<KType, DType, RType, PType>
CameraModelBase<KType, DType, RType, PType>::Scaled(double scale) const {
  CameraModelBase<KType, DType, RType, PType> scaled_model = *this;
  CHECK_GT(scale, 0.0);
  if (IsValidForProjection()) {
    // has only effect on K and P
    KType K;
    // KMatrixType K;
    ConstNativeMatrixRef<KMatrixType> K_ref(K_.matrix());
    if (!K_ref.empty()) {
      K = K_.Scaled(scale);
    }

    PType P;
    ConstNativeMatrixRef<PMatrixType> P_ref(P_.matrix());
    if (!P_ref.empty()) {
      P = P_.Scaled(scale);
    }
    scaled_model = CameraModelBase<KType, DType, RType, PType>(
        name_, image_size_ * scale, K, D_, R_, P);
  } else {
    LOG(WARNING)
        << "Trying to scale a camera model not valid! Ignoring scaling...";
  }
  return scaled_model;
}
template <typename KType, typename DType, typename RType, typename PType>
CameraModelBase<KType, DType, RType, PType>
CameraModelBase<KType, DType, RType, PType>::Roi(const Recti& roi) const {
  CameraModelBase<KType, DType, RType, PType> roi_model = *this;
  if (IsValidForProjection()) {
    // has only effect on cx and cy
    const Pointi& top_left = roi.top_left();
    Vector<ScalarType> vector{static_cast<ScalarType>(-top_left.x()),
                              static_cast<ScalarType>(-top_left.y())};

    KType K;
    ConstNativeMatrixRef<KMatrixType> K_ref(K_.matrix());
    if (!K_ref.empty()) {
      K = K_.CenterMoved(vector);
    }

    PType P;
    ConstNativeMatrixRef<PMatrixType> P_ref(P_.matrix());
    if (!P_ref.empty()) {
      P = P_.CenterMoved(vector);
    }
    roi_model = CameraModelBase<KType, DType, RType, PType>(name_, roi.size(),
                                                            K, D_, R_, P);
  } else {
    LOG(WARNING) << "Trying to extract roi from a camera model not valid! "
                    "Ignoring roi...";
  }
  return roi_model;
}

template <typename KType, typename DType, typename RType, typename PType>
double CameraModelBase<KType, DType, RType, PType>::HorizontalFOV() const {
  if (image_width() > 0 && fx() > 0.0) {
    return atan((static_cast<double>(image_width()) / 2.0) / fx()) * 2.0;
  }
  return 0.0;
}

template <typename KType, typename DType, typename RType, typename PType>
double CameraModelBase<KType, DType, RType, PType>::VerticalFOV() const {
  if (image_height() > 0 && fy() > 0.0) {
    return atan((static_cast<double>(image_height()) / 2.0) / fy()) * 2.0;
  }
  return 0.0;
}

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_CAMERA_CAMERA_MODEL_H_