#include "felicia/drivers/imu/madgwick_filter/madgwick_filter.h"

namespace felicia {

namespace {

// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
__attribute__((no_sanitize_address)) float InvSqrt(float x) {
  float halfx = 0.5f * x;
  float y = x;
  long i = *(long*)&y;
  i = 0x5f3759df - (i >> 1);
  y = *(float*)&i;
  y = y * (1.5f - (halfx * y * y));
  return y;
}

void Normalize(::Eigen::Quaternionf& q) {
  float r = InvSqrt(q.squaredNorm());
  q.vec() *= r;
  q.w() *= r;
}

template <typename Type, int Row, int Col>
void Normalize(::Eigen::Matrix<Type, Row, Col>& v) {
  float r = InvSqrt(v.squaredNorm());
  v *= r;
}

}  // namespace

MadgwickFilter::MadgwickFilter() : gradient_(::Eigen::Vector4f::Zero()) {
  gravity_.w() = 0;
  gravity_.vec() = ::Eigen::Vector3f(0, 0, 1);
}

::Eigen::Quaternionf MadgwickFilter::orientation() const {
  return orientation_;
}

void MadgwickFilter::UpdateAngularVelocity(float x, float y, float z,
                                           ::base::TimeDelta timestamp) {
  if (!has_measurement_ || last_timestamp_.is_zero()) {
    last_timestamp_ = timestamp;
    return;
  }

  double dt = (timestamp - last_timestamp_).InSecondsF();

  ::Eigen::Quaternionf q;
  q.w() = 0;
  q.vec() = ::Eigen::Vector3f(x, y, z) * 0.5;

  q = orientation_ * q;

  if (!gradient_.isZero()) {
    q.w() -= gradient_[0];
    q.x() -= gradient_[1];
    q.y() -= gradient_[2];
    q.z() -= gradient_[3];
    gradient_.setZero();
  }

  orientation_.vec() += q.vec() * dt;
  orientation_.w() += q.w() * dt;
  Normalize(orientation_);

  last_timestamp_ = timestamp;
}

void MadgwickFilter::UpdateLinearAcceleration(float x, float y, float z) {
  ::Eigen::Vector3f v;
  v << x, y, z;
  Normalize(v);

  if (!has_measurement_) {
    if (z >= 0) {
      double X = sqrt((v.z() + 1) * 0.5);
      orientation_.w() = X;
      orientation_.x() = -v.y() / (2 * X);
      orientation_.y() = v.x() / (2 * X);
      orientation_.z() = 0;
    } else {
      double X = sqrt((1 - v.z()) * 0.5);
      orientation_.w() = -v.y() / (2 * X);
      orientation_.x() = X;
      orientation_.y() = 0;
      orientation_.z() = v.x() / (2 * X);
    }

    has_measurement_ = true;
  } else {
    // Equation(12)
    ::Eigen::Vector3f f =
        (orientation_.inverse() * gravity_ * orientation_).vec() - v;

    float q0 = orientation_.w();
    float q1 = orientation_.x();
    float q2 = orientation_.y();
    float q3 = orientation_.z();

    // Equation(13)
    ::Eigen::Matrix<float, 4, 3> jacobian_T;
    jacobian_T << -2 * q2, 2 * q1, 0, 2 * q3, 2 * q0, -4 * q1, -2 * q0, 2 * q3,
        -4 * q2, 2 * q1, 2 * q2, 0;

    gradient_ = jacobian_T * f;
    Normalize(gradient_);
    gradient_ *= beta_;
  }
}

}  // namespace felicia