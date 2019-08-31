#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_RIGID_BODY_TRANSFORM_TRANSFORM_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_RIGID_BODY_TRANSFORM_TRANSFORM_H_

#include "Eigen/Geometry"

#if defined(HAS_OPENCV)
#include <opencv2/opencv.hpp>
#endif

namespace felicia {

template <typename RotationType, typename TranslationType>
class RigidBodyTransform;

template <typename T>
class RigidBodyTransform<Eigen::Matrix<T, 2, 2>, Eigen::Matrix<T, 2, 1>> {
 public:
  typedef T ScalarType;
  typedef Eigen::Matrix<T, 2, 2> RotationType;
  typedef Eigen::Matrix<T, 2, 1> TranslationType;
  typedef Eigen::Matrix<T, 2, 2> RotationMatrixType;
  typedef Eigen::Matrix<T, 2, 1> TranslationVectorType;

  RigidBodyTransform() = default;
  RigidBodyTransform(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_[0]; }
  ScalarType ty() const { return t_[1]; }

  RotationType& R() { return R_; }
  const RotationType& R() const { return R_; }

  TranslationType& t() { return t_; }
  const TranslationType& t() const { return t_; }

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

typedef RigidBodyTransform<Eigen::Matrix2f, Eigen::Vector2f>
    EigenRigidBodyTransformf;
typedef RigidBodyTransform<Eigen::Matrix2d, Eigen::Vector2d>
    EigenRigidBodyTransformd;

template <typename T>
class RigidBodyTransform<Eigen::Rotation2D<T>, Eigen::Translation<T, 2>> {
 public:
  typedef T ScalarType;
  typedef Eigen::Rotation2D<T> RotationType;
  typedef Eigen::Translation<T, 2> TranslationType;
  typedef Eigen::Matrix<T, 2, 2> RotationMatrixType;
  typedef Eigen::Matrix<T, 2, 1> TranslationVectorType;

  RigidBodyTransform() = default;
  RigidBodyTransform(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_.x(); }
  ScalarType ty() const { return t_.y(); }

  RotationType& R() { return R_; }
  const RotationType& R() const { return R_; }

  TranslationType& t() { return t_; }
  const TranslationType& t() const { return t_; }

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
class RigidBodyTransform<cv::Mat, cv::Mat> {
 public:
  typedef cv::Mat RotationType;
  typedef cv::Mat TranslationType;
  typedef cv::Mat RotationMatrixType;
  typedef cv::Mat TranslationVectorType;

  RigidBodyTransform() = default;
  RigidBodyTransform(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  // To conform to other RigidBodyTransform's
  double tx() const { return t_.template at<double>(0); }
  double ty() const { return t_.template at<double>(1); }
  double tz() const { return t_.template at<double>(2); }

  template <typename T,
            std::enable_if_t<!std::is_same<T, double>::value>* = nullptr>
  T tx() const {
    return t_.template at<T>(0);
  }
  template <typename T,
            std::enable_if_t<!std::is_same<T, double>::value>* = nullptr>
  T ty() const {
    return t_.template at<T>(1);
  }
  template <typename T,
            std::enable_if_t<!std::is_same<T, double>::value>* = nullptr>
  T tz() const {
    return t_.template at<T>(2);
  }

  RotationType& R() { return R_; }
  const RotationType& R() const { return R_; }

  TranslationType& t() { return t_; }
  const TranslationType& t() const { return t_; }

  RotationMatrixType& rotation_matrix() { return R_; }
  const RotationMatrixType& rotation_matrix() const { return R_; }

  TranslationVectorType& translation_vector() { return t_; }
  const TranslationVectorType& translation_vector() const { return t_; }

 private:
  RotationType R_;
  TranslationType t_;
};

template <typename T>
class RigidBodyTransform<cv::Mat_<T>, cv::Mat_<T>> {
 public:
  typedef T ScalarType;
  typedef cv::Mat_<T> RotationType;
  typedef cv::Mat_<T> TranslationType;
  typedef cv::Mat_<T> RotationMatrixType;
  typedef cv::Mat_<T> TranslationVectorType;

  RigidBodyTransform() = default;
  RigidBodyTransform(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_(0); }
  ScalarType ty() const { return t_(1); }

  RotationType& R() { return R_; }
  const RotationType& R() const { return R_; }

  TranslationType& t() { return t_; }
  const TranslationType& t() const { return t_; }

  RotationMatrixType& rotation_matrix() { return R_; }
  const RotationMatrixType& rotation_matrix() const { return R_; }

  TranslationVectorType& translation_vector() { return t_; }
  const TranslationVectorType& translation_vector() const { return t_; }

 private:
  RotationType R_;
  TranslationType t_;
};

typedef RigidBodyTransform<cv::Mat1f, cv::Mat1f> CvRigidBodyTransformf;
typedef RigidBodyTransform<cv::Mat1d, cv::Mat1d> CvRigidBodyTransformd;

template <typename T>
class RigidBodyTransform<cv::Matx<T, 2, 2>, cv::Matx<T, 2, 1>> {
 public:
  typedef T ScalarType;
  typedef cv::Matx<T, 2, 2> RotationType;
  typedef cv::Matx<T, 2, 1> TranslationType;
  typedef cv::Matx<T, 2, 2> RotationMatrixType;
  typedef cv::Matx<T, 2, 1> TranslationVectorType;

  RigidBodyTransform() = default;
  RigidBodyTransform(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_(0); }
  ScalarType ty() const { return t_(1); }
  ScalarType tz() const { return t_(2); }

  RotationType& R() { return R_; }
  const RotationType& R() const { return R_; }

  TranslationType& t() { return t_; }
  const TranslationType& t() const { return t_; }

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
class RigidBodyTransform3;

template <typename T>
class RigidBodyTransform3<Eigen::Matrix<T, 3, 3>, Eigen::Matrix<T, 3, 1>> {
 public:
  typedef T ScalarType;
  typedef Eigen::Matrix<T, 3, 3> RotationType;
  typedef Eigen::Matrix<T, 3, 1> TranslationType;
  typedef Eigen::Matrix<T, 3, 3> RotationMatrixType;
  typedef Eigen::Matrix<T, 3, 1> TranslationVectorType;

  RigidBodyTransform3() = default;
  RigidBodyTransform3(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_[0]; }
  ScalarType ty() const { return t_[1]; }
  ScalarType tz() const { return t_[2]; }

  RotationType& R() { return R_; }
  const RotationType& R() const { return R_; }

  TranslationType& t() { return t_; }
  const TranslationType& t() const { return t_; }

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

typedef RigidBodyTransform3<Eigen::Matrix3f, Eigen::Vector3f>
    EigenRigidBodyTransform3f;
typedef RigidBodyTransform3<Eigen::Matrix3d, Eigen::Vector3d>
    EigenRigidBodyTransform3d;

template <typename T>
class RigidBodyTransform3<Eigen::AngleAxis<T>, Eigen::Translation<T, 3>> {
 public:
  typedef T ScalarType;
  typedef Eigen::AngleAxis<T> RotationType;
  typedef Eigen::Translation<T, 3> TranslationType;
  typedef Eigen::Matrix<T, 3, 3> RotationMatrixType;
  typedef Eigen::Matrix<T, 3, 1> TranslationVectorType;

  RigidBodyTransform3() = default;
  RigidBodyTransform3(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_.x(); }
  ScalarType ty() const { return t_.y(); }
  ScalarType tz() const { return t_.z(); }

  RotationType& R() { return R_; }
  const RotationType& R() const { return R_; }

  TranslationType& t() { return t_; }
  const TranslationType& t() const { return t_; }

  RotationMatrixType rotation_matrix() const { return R_.matrix(); }

  TranslationVectorType translation_vector() const { return t_.vector(); }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF_VECTORIZABLE_FIXED_SIZE(
      TranslationType::Scalar, TranslationType::Dim)

 private:
  RotationType R_;
  TranslationType t_;
};

template <typename T>
class RigidBodyTransform3<Eigen::Quaternion<T>, Eigen::Translation<T, 3>> {
 public:
  typedef T ScalarType;
  typedef Eigen::Quaternion<T> RotationType;
  typedef Eigen::Translation<T, 3> TranslationType;
  typedef Eigen::Matrix<T, 3, 3> RotationMatrixType;
  typedef Eigen::Matrix<T, 3, 1> TranslationVectorType;

  RigidBodyTransform3() = default;
  RigidBodyTransform3(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_.x(); }
  ScalarType ty() const { return t_.y(); }
  ScalarType tz() const { return t_.z(); }

  RotationType& R() { return R_; }
  const RotationType& R() const { return R_; }

  TranslationType& t() { return t_; }
  const TranslationType& t() const { return t_; }

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
class RigidBodyTransform3<cv::Mat, cv::Mat> {
 public:
  typedef cv::Mat RotationType;
  typedef cv::Mat TranslationType;
  typedef cv::Mat RotationMatrixType;
  typedef cv::Mat TranslationVectorType;

  RigidBodyTransform3() = default;
  RigidBodyTransform3(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  // To conform to other RigidBodyTransform's
  double tx() const { return t_.template at<double>(0); }
  double ty() const { return t_.template at<double>(1); }
  double tz() const { return t_.template at<double>(2); }

  template <typename T,
            std::enable_if_t<!std::is_same<T, double>::value>* = nullptr>
  T tx() const {
    return t_.template at<T>(0);
  }
  template <typename T,
            std::enable_if_t<!std::is_same<T, double>::value>* = nullptr>
  T ty() const {
    return t_.template at<T>(1);
  }
  template <typename T,
            std::enable_if_t<!std::is_same<T, double>::value>* = nullptr>
  T tz() const {
    return t_.template at<T>(2);
  }

  RotationType& R() { return R_; }
  const RotationType& R() const { return R_; }

  TranslationType& t() { return t_; }
  const TranslationType& t() const { return t_; }

  RotationMatrixType& rotation_matrix() { return R_; }
  const RotationMatrixType& rotation_matrix() const { return R_; }

  TranslationVectorType& translation_vector() { return t_; }
  const TranslationVectorType& translation_vector() const { return t_; }

 private:
  RotationType R_;
  TranslationType t_;
};

template <typename T>
class RigidBodyTransform3<cv::Mat_<T>, cv::Mat_<T>> {
 public:
  typedef T ScalarType;
  typedef cv::Mat_<T> RotationType;
  typedef cv::Mat_<T> TranslationType;
  typedef cv::Mat_<T> RotationMatrixType;
  typedef cv::Mat_<T> TranslationVectorType;

  RigidBodyTransform3() = default;
  RigidBodyTransform3(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_(0); }
  ScalarType ty() const { return t_(1); }
  ScalarType tz() const { return t_(2); }

  RotationType& R() { return R_; }
  const RotationType& R() const { return R_; }

  TranslationType& t() { return t_; }
  const TranslationType& t() const { return t_; }

  RotationMatrixType& rotation_matrix() { return R_; }
  const RotationMatrixType& rotation_matrix() const { return R_; }

  TranslationVectorType& translation_vector() { return t_; }
  const TranslationVectorType& translation_vector() const { return t_; }

 private:
  RotationType R_;
  TranslationType t_;
};

typedef RigidBodyTransform3<cv::Mat1f, cv::Mat1f> CvRigidBodyTransform3f;
typedef RigidBodyTransform3<cv::Mat1d, cv::Mat1d> CvRigidBodyTransform3d;

template <typename T>
class RigidBodyTransform3<cv::Matx<T, 3, 3>, cv::Matx<T, 3, 1>> {
 public:
  typedef T ScalarType;
  typedef cv::Matx<T, 3, 3> RotationType;
  typedef cv::Matx<T, 3, 1> TranslationType;
  typedef cv::Matx<T, 3, 3> RotationMatrixType;
  typedef cv::Matx<T, 3, 1> TranslationVectorType;

  RigidBodyTransform3() = default;
  RigidBodyTransform3(const RotationType& R, const TranslationType& t)
      : R_(R), t_(t) {}

  ScalarType tx() const { return t_(0); }
  ScalarType ty() const { return t_(1); }
  ScalarType tz() const { return t_(2); }

  RotationType& R() { return R_; }
  const RotationType& R() const { return R_; }

  TranslationType& t() { return t_; }
  const TranslationType& t() const { return t_; }

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

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_RIGID_BODY_TRANSFORM_TRANSFORM_H_