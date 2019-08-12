#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_FACTORABLE_NATIVE_TRANSFORM_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_FACTORABLE_NATIVE_TRANSFORM_H_

#include "Eigen/Geometry"

#if defined(HAS_OPENCV)
#include <opencv2/opencv.hpp>
#endif

namespace felicia {

template <typename RotationType, typename TranslationType>
class FactorableNativeTransform;

template <typename T>
class FactorableNativeTransform<Eigen::Matrix<T, 2, 2>,
                                Eigen::Matrix<T, 2, 1>> {
 public:
  typedef T ScalarType;
  typedef Eigen::Matrix<T, 2, 2> RotationType;
  typedef Eigen::Matrix<T, 2, 1> TranslationType;
  typedef Eigen::Matrix<T, 2, 2> RotationMatrixType;
  typedef Eigen::Matrix<T, 2, 1> TranslationVectorType;

  FactorableNativeTransform(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_[0]; }
  ScalarType ty() const { return t_[1]; }

  RotationType& rotation() { return R_; }
  const RotationType& rotation() const { return R_; }

  TranslationType& translation() { return t_; }
  const TranslationType& translation() const { return t_; }

  RotationMatrixType& rotation_matrix() { return R_; }
  const RotationMatrixType& rotation_matrix() const { return R_; }

  TranslationVectorType& translation_vector() { return t_; }
  const TranslationVectorType& translation_vector() const { return t_; }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF(bool(RotationType::NeedsToAlign) ||
                                     bool(TranslationType::NeedsToAlign))

 private:
  RotationType R_;
  TranslationType t_;
};

template <typename T>
class FactorableNativeTransform<Eigen::Rotation2D<T>,
                                Eigen::Translation<T, 2>> {
 public:
  typedef T ScalarType;
  typedef Eigen::Rotation2D<T> RotationType;
  typedef Eigen::Translation<T, 2> TranslationType;
  typedef Eigen::Matrix<T, 2, 2> RotationMatrixType;
  typedef Eigen::Matrix<T, 2, 1> TranslationVectorType;

  FactorableNativeTransform(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_.x(); }
  ScalarType ty() const { return t_.y(); }

  RotationType& rotation() { return R_; }
  const RotationType& rotation() const { return R_; }

  TranslationType& translation() { return t_; }
  const TranslationType& translation() const { return t_; }

  RotationMatrixType rotation_matrix() const { return R_.matrix(); }

  TranslationVectorType translation_vector() const { return t_.vector(); }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF_VECTORIZABLE_FIXED_SIZE(
      TranslationType::Scalar, TranslationType::Dim)

 private:
  RotationType R_;
  TranslationType t_;
};

#if defined(HAS_OPENCV)
template <>
class FactorableNativeTransform<cv::Mat, cv::Mat> {
 public:
  typedef double ScalarType;
  typedef cv::Mat RotationType;
  typedef cv::Mat TranslationType;
  typedef cv::Mat RotationMatrixType;
  typedef cv::Mat TranslationVectorType;

  FactorableNativeTransform(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_.template at<ScalarType>(0); }
  ScalarType ty() const { return t_.template at<ScalarType>(1); }
  ScalarType tz() const { return t_.template at<ScalarType>(2); }

  RotationType& rotation() { return R_; }
  const RotationType& rotation() const { return R_; }

  TranslationType& translation() { return t_; }
  const TranslationType& translation() const { return t_; }

  RotationMatrixType& rotation_matrix() { return R_; }
  const RotationMatrixType& rotation_matrix() const { return R_; }

  TranslationVectorType& translation_vector() { return t_; }
  const TranslationVectorType& translation_vector() const { return t_; }

 private:
  RotationType R_;
  TranslationType t_;
};

template <typename T>
class FactorableNativeTransform<cv::Mat_<T>, cv::Mat_<T>> {
 public:
  typedef T ScalarType;
  typedef cv::Mat_<T> RotationType;
  typedef cv::Mat_<T> TranslationType;
  typedef cv::Mat_<T> RotationMatrixType;
  typedef cv::Mat_<T> TranslationVectorType;

  FactorableNativeTransform(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_.template at<ScalarType>(0); }
  ScalarType ty() const { return t_.template at<ScalarType>(1); }

  RotationType& rotation() { return R_; }
  const RotationType& rotation() const { return R_; }

  TranslationType& translation() { return t_; }
  const TranslationType& translation() const { return t_; }

  RotationMatrixType& rotation_matrix() { return R_; }
  const RotationMatrixType& rotation_matrix() const { return R_; }

  TranslationVectorType& translation_vector() { return t_; }
  const TranslationVectorType& translation_vector() const { return t_; }

 private:
  RotationType R_;
  TranslationType t_;
};

template <typename T>
class FactorableNativeTransform<cv::Matx<T, 2, 2>, cv::Matx<T, 2, 1>> {
 public:
  typedef T ScalarType;
  typedef cv::Matx<T, 2, 2> RotationType;
  typedef cv::Matx<T, 2, 1> TranslationType;
  typedef cv::Matx<T, 2, 2> RotationMatrixType;
  typedef cv::Matx<T, 2, 1> TranslationVectorType;

  FactorableNativeTransform(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_.template at<ScalarType>(0); }
  ScalarType ty() const { return t_.template at<ScalarType>(1); }
  ScalarType tz() const { return t_.template at<ScalarType>(2); }

  RotationType& rotation() { return R_; }
  const RotationType& rotation() const { return R_; }

  TranslationType& translation() { return t_; }
  const TranslationType& translation() const { return t_; }

  RotationMatrixType& rotation_matrix() { return R_; }
  const RotationMatrixType& rotation_matrix() const { return R_; }

  TranslationVectorType& translation_vector() { return t_; }
  const TranslationVectorType& translation_vector() const { return t_; }

 private:
  RotationType R_;
  TranslationType t_;
};
#endif

template <typename RotationType, typename TranslationType>
class FactorableNativeTransform3;

template <typename T>
class FactorableNativeTransform3<Eigen::Matrix<T, 3, 3>,
                                 Eigen::Matrix<T, 3, 1>> {
 public:
  typedef T ScalarType;
  typedef Eigen::Matrix<T, 3, 3> RotationType;
  typedef Eigen::Matrix<T, 3, 1> TranslationType;
  typedef Eigen::Matrix<T, 3, 3> RotationMatrixType;
  typedef Eigen::Matrix<T, 3, 1> TranslationVectorType;

  FactorableNativeTransform3(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_[0]; }
  ScalarType ty() const { return t_[1]; }
  ScalarType tz() const { return t_[2]; }

  RotationType& rotation() { return R_; }
  const RotationType& rotation() const { return R_; }

  TranslationType& translation() { return t_; }
  const TranslationType& translation() const { return t_; }

  RotationMatrixType& rotation_matrix() { return R_; }
  const RotationMatrixType& rotation_matrix() const { return R_; }

  TranslationVectorType& translation_vector() { return t_; }
  const TranslationVectorType& translation_vector() const { return t_; }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF(bool(RotationType::NeedsToAlign) ||
                                     bool(TranslationType::NeedsToAlign))

 private:
  RotationType R_;
  TranslationType t_;
};

template <typename T>
class FactorableNativeTransform3<Eigen::AngleAxis<T>,
                                 Eigen::Translation<T, 3>> {
 public:
  typedef T ScalarType;
  typedef Eigen::AngleAxis<T> RotationType;
  typedef Eigen::Translation<T, 3> TranslationType;
  typedef Eigen::Matrix<T, 3, 3> RotationMatrixType;
  typedef Eigen::Matrix<T, 3, 1> TranslationVectorType;

  FactorableNativeTransform3(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_.x(); }
  ScalarType ty() const { return t_.y(); }
  ScalarType tz() const { return t_.z(); }

  RotationType& rotation() { return R_; }
  const RotationType& rotation() const { return R_; }

  TranslationType& translation() { return t_; }
  const TranslationType& translation() const { return t_; }

  RotationMatrixType rotation_matrix() const { return R_.matrix(); }

  TranslationVectorType translation_vector() const { return t_.vector(); }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF_VECTORIZABLE_FIXED_SIZE(
      TranslationType::Scalar, TranslationType::Dim)

 private:
  RotationType R_;
  TranslationType t_;
};

template <typename T>
class FactorableNativeTransform3<Eigen::Quaternion<T>,
                                 Eigen::Translation<T, 3>> {
 public:
  typedef T ScalarType;
  typedef Eigen::Quaternion<T> RotationType;
  typedef Eigen::Translation<T, 3> TranslationType;
  typedef Eigen::Matrix<T, 3, 3> RotationMatrixType;
  typedef Eigen::Matrix<T, 3, 1> TranslationVectorType;

  FactorableNativeTransform3(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_.x(); }
  ScalarType ty() const { return t_.y(); }
  ScalarType tz() const { return t_.z(); }

  RotationType& rotation() { return R_; }
  const RotationType& rotation() const { return R_; }

  TranslationType& translation() { return t_; }
  const TranslationType& translation() const { return t_; }

  RotationMatrixType rotation_matrix() const { return R_.matrix(); }

  TranslationVectorType translation_vector() const { return t_.vector(); }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF(
      bool(RotationType::NeedsAlignment) ||
      bool(((TranslationType::Dim) != Eigen::Dynamic) &&
           ((sizeof(TranslationType::Scalar) * (TranslationType::Dim)) %
                EIGEN_MAX_ALIGN_BYTES ==
            0)))

 private:
  RotationType R_;
  TranslationType t_;
};

#if defined(HAS_OPENCV)
template <>
class FactorableNativeTransform3<cv::Mat, cv::Mat> {
 public:
  typedef double ScalarType;
  typedef cv::Mat RotationType;
  typedef cv::Mat TranslationType;
  typedef cv::Mat RotationMatrixType;
  typedef cv::Mat TranslationVectorType;

  FactorableNativeTransform3(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_.template at<ScalarType>(0); }
  ScalarType ty() const { return t_.template at<ScalarType>(1); }
  ScalarType tz() const { return t_.template at<ScalarType>(2); }

  RotationType& rotation() { return R_; }
  const RotationType& rotation() const { return R_; }

  TranslationType& translation() { return t_; }
  const TranslationType& translation() const { return t_; }

  RotationMatrixType& rotation_matrix() { return R_; }
  const RotationMatrixType& rotation_matrix() const { return R_; }

  TranslationVectorType& translation_vector() { return t_; }
  const TranslationVectorType& translation_vector() const { return t_; }

 private:
  RotationType R_;
  TranslationType t_;
};

template <typename T>
class FactorableNativeTransform3<cv::Mat_<T>, cv::Mat_<T>> {
 public:
  typedef T ScalarType;
  typedef cv::Mat_<T> RotationType;
  typedef cv::Mat_<T> TranslationType;
  typedef cv::Mat_<T> RotationMatrixType;
  typedef cv::Mat_<T> TranslationVectorType;

  FactorableNativeTransform3(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_.template at<ScalarType>(0); }
  ScalarType ty() const { return t_.template at<ScalarType>(1); }
  ScalarType tz() const { return t_.template at<ScalarType>(2); }

  RotationType& rotation() { return R_; }
  const RotationType& rotation() const { return R_; }

  TranslationType& translation() { return t_; }
  const TranslationType& translation() const { return t_; }

  RotationMatrixType& rotation_matrix() { return R_; }
  const RotationMatrixType& rotation_matrix() const { return R_; }

  TranslationVectorType& translation_vector() { return t_; }
  const TranslationVectorType& translation_vector() const { return t_; }

 private:
  RotationType R_;
  TranslationType t_;
};

template <typename T>
class FactorableNativeTransform3<cv::Matx<T, 3, 3>, cv::Matx<T, 3, 1>> {
 public:
  typedef T ScalarType;
  typedef cv::Matx<T, 3, 3> RotationType;
  typedef cv::Matx<T, 3, 1> TranslationType;
  typedef cv::Matx<T, 3, 3> RotationMatrixType;
  typedef cv::Matx<T, 3, 1> TranslationVectorType;

  FactorableNativeTransform3(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_.template at<ScalarType>(0); }
  ScalarType ty() const { return t_.template at<ScalarType>(1); }
  ScalarType tz() const { return t_.template at<ScalarType>(2); }

  RotationType& rotation() { return R_; }
  const RotationType& rotation() const { return R_; }

  TranslationType& translation() { return t_; }
  const TranslationType& translation() const { return t_; }

  RotationMatrixType& rotation_matrix() { return R_; }
  const RotationMatrixType& rotation_matrix() const { return R_; }

  TranslationVectorType& translation_vector() { return t_; }
  const TranslationVectorType& translation_vector() const { return t_; }

 private:
  RotationType R_;
  TranslationType t_;
};
#endif

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_FACTORABLE_NATIVE_TRANSFORM_H_