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

#ifndef FELICIA_SLAM_CAMERA_STEREO_CAMERA_MODEL_BASE_H_
#define FELICIA_SLAM_CAMERA_STEREO_CAMERA_MODEL_BASE_H_

#include "third_party/chromium/base/files/file_path.h"
#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/slam/camera/camera_model_base.h"

namespace felicia {
namespace slam {

template <typename CameraModelType, typename TType, typename EType,
          typename FType>
class StereoCameraModelBase {
 public:
  typedef typename TType::RotationMatrixType RotationMatrixType;
  typedef typename TType::TranslationVectorType TranslationVectorType;
  typedef typename EType::MatrixType EMatrixType;
  typedef typename FType::MatrixType FMatrixType;
  typedef typename TType::ScalarType ScalarType;
  static_assert(std::is_same<typename CameraModelType::ScalarType,
                             typename TType::ScalarType>::value &&
                    std::is_same<typename TType::ScalarType,
                                 typename EType::ScalarType>::value &&
                    std::is_same<typename EType::ScalarType,
                                 typename FType::ScalarType>::value,
                "All ScalarTypes should be same");

  StereoCameraModelBase() = default;

  StereoCameraModelBase(const std::string& name,
                        const CameraModelType& left_camera_model,
                        const CameraModelType& right_camera_model,
                        const RotationMatrixType& R,
                        const TranslationVectorType& t, const EMatrixType& E,
                        const FMatrixType& F)
      : name_(name),
        left_camera_model_(left_camera_model),
        right_camera_model_(right_camera_model),
        T_(R, t),
        E_(E),
        F_(F) {}

  StereoCameraModelBase(const std::string& name,
                        const CameraModelType& left_camera_model,
                        const CameraModelType& right_camera_model,
                        const TType& T, const EType& E, const FType& F)
      : name_(name),
        left_camera_model_(left_camera_model),
        right_camera_model_(right_camera_model),
        T_(T),
        E_(E),
        F_(F) {}

  // minimal
  StereoCameraModelBase(ScalarType fx, ScalarType fy, ScalarType cx,
                        ScalarType cy, ScalarType baseline,
                        const Sizei& image_size = Sizei(0, 0))
      : StereoCameraModelBase(base::EmptyString(), base::EmptyString(),
                              base::EmptyString(), fx, fy, cx, cy, baseline,
                              image_size) {}

  // minimal to be saved
  StereoCameraModelBase(const std::string& name,
                        const std::string& left_camera_model_name,
                        const std::string& right_camera_model_name,
                        ScalarType fx, ScalarType fy, ScalarType cx,
                        ScalarType cy, ScalarType baseline,
                        const Sizei& image_size = Sizei(0, 0))
      : name_(name),
        left_camera_model_(left_camera_model_name, fx, fy, cx, cy, 0,
                           image_size),
        right_camera_model_(right_camera_model_name, fx, fy, cx, cy,
                            baseline * -fx, image_size) {}

  ~StereoCameraModelBase() = default;

  bool IsValidForProjection() const {
    return left_camera_model_.IsValidForProjection() &&
           right_camera_model_.IsValidForProjection() && baseline() > 0.0;
  }
  bool IsValidForRectification() const {
    return left_camera_model_.IsValidForRectification() &&
           right_camera_model_.IsValidForRectification();
  }

  void set_name(const std::string& name) { name_ = name; }
  const std::string& name() const { return name_; }

  const CameraModelType& left_camera_model() const {
    return left_camera_model_;
  }
  const CameraModelType& right_camera_model() const {
    return right_camera_model_;
  }

  RotationMatrixType R() const { return T_.rotation_matrix(); }
  TranslationVectorType t() const { return T_.translation_vector(); }
  const TType& T() const { return T_; }
  const EType& E() const { return E_; }
  const FType& F() const { return F_; }

  Status Load(const base::FilePath& left_camera_model_path,
              const base::FilePath& right_camera_model_path,
              const base::FilePath& stereo_transform_path = base::FilePath());
  Status Save(
      const base::FilePath& left_camera_model_path,
      const base::FilePath& right_camera_model_path,
      const base::FilePath& stereo_transform_path = base::FilePath()) const;
  Status LoadStereoTransform(const base::FilePath& path);
  Status SaveStereoTransform(const base::FilePath& path) const;

  StereoCameraModelMessage ToStereoCameraModelMessage() const;
  Status FromStereoCameraModelMessage(const StereoCameraModelMessage& message);
  Status FromStereoCameraModelMessage(StereoCameraModelMessage&& message);

  double baseline() const {
    return right_camera_model_.fx() != 0.0 && left_camera_model_.fx() != 0.0
               ? left_camera_model_.tx() / left_camera_model_.fx() -
                     right_camera_model_.tx() / right_camera_model_.fx()
               : 0.0;
  }

  ScalarType ComputeDepth(ScalarType disparity) const {
    // depth = baseline * f / (disparity + cx1 - cx0);
    CHECK(IsValidForProjection());
    if (disparity == 0) {
      return 0;
    }
    return baseline() * left_camera_model_.fx() /
           (disparity + right_camera_model_.cx() - left_camera_model_.cx());
  }
  ScalarType ComputeDisparity(ScalarType depth) const {
    // disparity = (baseline * fx / depth) - (cx1 - cx0);
    CHECK(IsValidForProjection());
    if (depth == 0) {
      return 0;
    }
    return baseline() * left_camera_model_.fx() / depth -
           (right_camera_model_.cx() - left_camera_model_.cx());
  }

 private:
  std::string name_;
  CameraModelType left_camera_model_;
  CameraModelType right_camera_model_;
  // The rigid body transform 3x4 matrix
  TType T_;
  // The essential matrix 3x3
  EType E_;
  // The fundamental matrix 3x3
  FType F_;
};

template <typename CameraModelType, typename TType, typename EType,
          typename FType>
Status StereoCameraModelBase<CameraModelType, TType, EType, FType>::Load(
    const base::FilePath& left_camera_model_path,
    const base::FilePath& right_camera_model_path,
    const base::FilePath& stereo_transform_path) {
  Status s = left_camera_model_.Load(left_camera_model_path);
  if (!s.ok()) return s;

  s = right_camera_model_.Load(right_camera_model_path);
  if (!s.ok()) return s;

  if (stereo_transform_path.empty()) return Status::OK();

  return LoadStereoTransform(stereo_transform_path);
}

template <typename CameraModelType, typename TType, typename EType,
          typename FType>
Status StereoCameraModelBase<CameraModelType, TType, EType, FType>::Save(
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

template <typename CameraModelType, typename TType, typename EType,
          typename FType>
Status
StereoCameraModelBase<CameraModelType, TType, EType,
                      FType>::LoadStereoTransform(const base::FilePath& path) {
  YamlReader reader;
  Status s = reader.Open(path);
  if (!s.ok()) return s;
  const YAML::Node& node = reader.node();

  std::string name;
  RotationMatrixType R;
  TranslationVectorType t;
  EType E;
  FType F;

  internal::MaybeLoad(node, "camera_name", path, [&name](const YAML::Node& n) {
    name = n.as<std::string>();
    return Status::OK();
  });

  // import from ROS calibration format
  s = internal::MaybeLoad(
      node, "rotation_matrix", path, [&R](const YAML::Node& n) {
        StatusOr<RotationMatrixType> status_or =
            internal::LoadMatrix<RotationMatrixType>(n, IsMatrix<3, 3>);
        if (!status_or.ok()) return status_or.status();
        R = status_or.ValueOrDie();
        return Status::OK();
      });
  if (!s.ok()) return s;

  s = internal::MaybeLoad(
      node, "translation_matrix", path, [&t](const YAML::Node& n) {
        StatusOr<TranslationVectorType> status_or =
            internal::LoadMatrix<TranslationVectorType>(n, IsMatrix<3, 1>);
        if (!status_or.ok()) return status_or.status();
        t = status_or.ValueOrDie();
        return Status::OK();
      });
  if (!s.ok()) return s;

  s = internal::MaybeLoad(
      node, "essential_matrix", path, [&E](const YAML::Node& n) {
        StatusOr<EMatrixType> status_or =
            internal::LoadMatrix<EMatrixType>(n, IsMatrix<3, 3>);
        if (!status_or.ok()) return status_or.status();
        E = status_or.ValueOrDie();
        return Status::OK();
      });
  if (!s.ok()) return s;

  s = internal::MaybeLoad(
      node, "fundamental_matrix", path, [&F](const YAML::Node& n) {
        StatusOr<FMatrixType> status_or =
            internal::LoadMatrix<FMatrixType>(n, IsMatrix<3, 3>);
        if (!status_or.ok()) return status_or.status();
        F = status_or.ValueOrDie();
        return Status::OK();
      });
  if (!s.ok()) return s;

  name_ = name;
  T_ = TType{R, t};
  E_ = E;
  F_ = F;

  return Status::OK();
}

template <typename CameraModelType, typename TType, typename EType,
          typename FType>
Status StereoCameraModelBase<CameraModelType, TType, EType, FType>::
    SaveStereoTransform(const base::FilePath& path) const {
  RotationMatrixType R = T_.R();
  TranslationVectorType t = T_.t();
  ConstNativeMatrixRef<RotationMatrixType> R_ref(R);
  ConstNativeMatrixRef<TranslationVectorType> t_ref(t);
  ConstNativeMatrixRef<EMatrixType> E_ref(E_.matrix());
  ConstNativeMatrixRef<FMatrixType> F_ref(F_.matrix());
  if (!R_ref.empty() && !t_ref.empty()) {
    YamlWriter yaml_writer;
    YAML::Emitter& emitter = yaml_writer.emitter();

    // export in ROS calibration format
    emitter << YAML::BeginMap;
    if (!name_.empty()) {
      emitter << YAML::Key << "camera_name" << YAML::Value << name_;
    }

    if (!R_ref.empty()) {
      emitter << YAML::Key << "rotation_matrix";
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

    if (!t_ref.empty()) {
      emitter << YAML::Key << "translation_matrix";
      emitter << YAML::BeginMap;
      emitter << YAML::Key << "rows" << YAML::Value << t_ref.rows();
      emitter << YAML::Key << "cols" << YAML::Value << t_ref.cols();
      emitter << YAML::Key << "data" << YAML::Value << YAML::Flow
              << std::vector<ScalarType>(
                     reinterpret_cast<const ScalarType*>(t_ref.data()),
                     reinterpret_cast<const ScalarType*>(t_ref.data()) +
                         t_ref.size());
      emitter << YAML::EndMap;
    }

    if (!E_ref.empty()) {
      emitter << YAML::Key << "essential_matrix";
      emitter << YAML::BeginMap;
      emitter << YAML::Key << "rows" << YAML::Value << E_ref.rows();
      emitter << YAML::Key << "cols" << YAML::Value << E_ref.cols();
      emitter << YAML::Key << "data" << YAML::Value << YAML::Flow
              << std::vector<ScalarType>(
                     reinterpret_cast<const ScalarType*>(E_ref.data()),
                     reinterpret_cast<const ScalarType*>(E_ref.data()) +
                         E_ref.size());
      emitter << YAML::EndMap;
    }

    if (!F_ref.empty()) {
      emitter << YAML::Key << "fundamental_matrix";
      emitter << YAML::BeginMap;
      emitter << YAML::Key << "rows" << YAML::Value << F_ref.rows();
      emitter << YAML::Key << "cols" << YAML::Value << F_ref.cols();
      emitter << YAML::Key << "data" << YAML::Value << YAML::Flow
              << std::vector<ScalarType>(
                     reinterpret_cast<const ScalarType*>(F_ref.data()),
                     reinterpret_cast<const ScalarType*>(F_ref.data()) +
                         F_ref.size());
      emitter << YAML::EndMap;
    }
    emitter << YAML::EndMap;

    Status s = yaml_writer.WriteToFile(path);
    if (!s.ok()) return s;
  } else {
    return errors::InvalidArgument(
        "Cannot save stereo extrinsics, check if R and t are not empty");
  }
  return Status::OK();
}

template <typename CameraModelType, typename TType, typename EType,
          typename FType>
StereoCameraModelMessage StereoCameraModelBase<
    CameraModelType, TType, EType, FType>::ToStereoCameraModelMessage() const {
  StereoCameraModelMessage message;
  *message.mutable_left_camera_model() =
      left_camera_model_.ToCameraModelMessage();
  *message.mutable_right_camera_model() =
      right_camera_model_.ToCameraModelMessage();
  RotationMatrixType R = T_.R();
  TranslationVectorType t = T_.t();
  ConstNativeMatrixRef<RotationMatrixType> R_ref(R);
  ConstNativeMatrixRef<TranslationVectorType> t_ref(t);
  ConstNativeMatrixRef<EMatrixType> E_ref(E_.matrix());
  ConstNativeMatrixRef<FMatrixType> F_ref(F_.matrix());
  if (!R_ref.empty()) message.set_r(R_ref.data(), R_ref.allocation_size());
  if (!t_ref.empty()) message.set_t(t_ref.data(), t_ref.allocation_size());
  if (!E_ref.empty()) message.set_e(E_ref.data(), E_ref.allocation_size());
  if (!F_ref.empty()) message.set_f(F_ref.data(), F_ref.allocation_size());

  return message;
}

template <typename CameraModelType, typename TType, typename EType,
          typename FType>
Status StereoCameraModelBase<CameraModelType, TType, EType, FType>::
    FromStereoCameraModelMessage(const StereoCameraModelMessage& message) {
  CameraModelType left_camera_model;
  CameraModelType right_camera_model;
  RotationMatrixType R;
  TranslationVectorType t;
  EType E;
  FType F;

  if (message.has_left_camera_model()) {
    Status s =
        left_camera_model.FromCameraModelMessage(message.left_camera_model());
    if (!s.ok()) return s;
  }

  if (message.has_right_camera_model()) {
    Status s =
        right_camera_model.FromCameraModelMessage(message.right_camera_model());
    if (!s.ok()) return s;
  }

  if (!message.r().empty()) {
    const std::string& r = message.r();
    int rows = 3;
    int cols = r.length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    R = ConstNativeMatrixRef<RotationMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(r.c_str())), rows,
        cols);
  }

  if (!message.t().empty()) {
    const std::string& t2 = message.t();
    int rows = 3;
    int cols = t2.length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 1>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    t = ConstNativeMatrixRef<TranslationVectorType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(t2.c_str())), rows,
        cols);
  }

  if (!message.e().empty()) {
    const std::string& e = message.e();
    int rows = 3;
    int cols = e.length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    E = ConstNativeMatrixRef<EMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(e.c_str())), rows,
        cols);
  }

  if (!message.f().empty()) {
    const std::string& f = message.f();
    int rows = 3;
    int cols = f.length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    F = ConstNativeMatrixRef<FMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(f.c_str())), rows,
        cols);
  }

  left_camera_model_ = left_camera_model;
  right_camera_model_ = right_camera_model;
  T_ = TType{R, t};
  E_ = E;
  F_ = F;

  return Status::OK();
}

template <typename CameraModelType, typename TType, typename EType,
          typename FType>
Status StereoCameraModelBase<CameraModelType, TType, EType, FType>::
    FromStereoCameraModelMessage(StereoCameraModelMessage&& message) {
  CameraModelType left_camera_model;
  CameraModelType right_camera_model;
  RotationMatrixType R;
  TranslationVectorType t;
  EType E;
  FType F;

  if (message.has_left_camera_model()) {
    std::unique_ptr<CameraModelMessage> left_camera_model2(
        message.release_left_camera_model());
    Status s = left_camera_model.FromCameraModelMessage(
        std::move(*left_camera_model2));
    if (!s.ok()) return s;
  }

  if (message.has_right_camera_model()) {
    std::unique_ptr<CameraModelMessage> right_camera_model2(
        message.release_right_camera_model());
    Status s = right_camera_model.FromCameraModelMessage(
        std::move(*right_camera_model2));
    if (!s.ok()) return s;
  }

  if (!message.r().empty()) {
    std::unique_ptr<std::string> r(message.release_r());
    int rows = 3;
    int cols = r->length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    R = ConstNativeMatrixRef<RotationMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(r->c_str())), rows,
        cols);
  }

  if (!message.t().empty()) {
    std::unique_ptr<std::string> t2(message.release_t());
    int rows = 3;
    int cols = t2->length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 1>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    t = ConstNativeMatrixRef<TranslationVectorType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(t2->c_str())), rows,
        cols);
  }

  if (!message.e().empty()) {
    std::unique_ptr<std::string> e(message.release_e());
    int rows = 3;
    int cols = e->length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    E = ConstNativeMatrixRef<EMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(e->c_str())), rows,
        cols);
  }

  if (!message.f().empty()) {
    std::unique_ptr<std::string> f(message.release_f());
    int rows = 3;
    int cols = f->length() / (rows * sizeof(ScalarType));
    if (!IsMatrix<3, 3>(rows, cols))
      return internal::InvalidRowsAndCols(rows, cols);
    F = ConstNativeMatrixRef<FMatrixType>::NewMatrix(
        reinterpret_cast<ScalarType*>(const_cast<char*>(f->c_str())), rows,
        cols);
  }

  left_camera_model_ = left_camera_model;
  right_camera_model_ = right_camera_model;
  T_ = TType{R, t};
  E_ = E;
  F_ = F;

  return Status::OK();
}

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_CAMERA_STEREO_CAMERA_MODEL_BASE_H_