#include "felicia/drivers/imu/madgwick_filter/madgwick_filter.h"

namespace felicia {
namespace drivers {

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

void Normalize(Quaternionf& q) {
  float r = InvSqrt(q.SquaredNorm());
  q.ScaleInPlace(r);
}

void Normalize(Vector3f& v) {
  float r = InvSqrt(v.SquaredNorm());
  v.ScaleInPlace(r);
}

template <typename Type, int Row, int Col>
void Normalize(Eigen::Matrix<Type, Row, Col>& v) {
  float r = InvSqrt(v.squaredNorm());
  v *= r;
}

}  // namespace

MadgwickFilter::MadgwickFilter()
    : gravity_(0, 0, 1, 0), gradient_(Eigen::Vector4f::Zero()) {}

Quaternionf MadgwickFilter::orientation() const {
  return orientation_.inverse();
}

void MadgwickFilter::UpdateAngularVelocity(const Vector3f& angular_velocity,
                                           base::TimeDelta timestamp) {
  if (!has_measurement_ || last_timestamp_.is_zero()) {
    last_timestamp_ = timestamp;
    return;
  }

  double dt = (timestamp - last_timestamp_).InSecondsF();

  Quaternionf q;
  q.set_vector(angular_velocity.Scale(0.5));
  q.set_w(0);
  q = orientation_ * q;

  if (!gradient_.isZero()) {
    Quaternionf g(gradient_[1], gradient_[2], gradient_[3], gradient_[0]);
    q -= g;
    gradient_.setZero();
  }

  orientation_ += q.ScaleInPlace(static_cast<float>(dt));
  Normalize(orientation_);

  last_timestamp_ = timestamp;
}

void MadgwickFilter::UpdateLinearAcceleration(
    const Vector3f& linear_acceleration) {
  Vector3f v = linear_acceleration;
  Normalize(v);

  if (!has_measurement_) {
    if (v.z() >= 0) {
      double X = sqrt((v.z() + 1) * 0.5);
      orientation_.set_xyzw(-v.y() / (2 * X), v.x() / (2 * X), 0, X);
    } else {
      double X = sqrt((1 - v.z()) * 0.5);
      orientation_.set_xyzw(X, 0, v.x() / (2 * X), -v.y() / (2 * X));
    }

    has_measurement_ = true;
  } else {
    // Equation(12)
    Vector3f f =
        (orientation_.inverse() * gravity_ * orientation_).vector() - v;

    float q0 = orientation_.w();
    float q1 = orientation_.x();
    float q2 = orientation_.y();
    float q3 = orientation_.z();

    // Equation(13)
    Eigen::Matrix<float, 4, 3> jacobian_T;
    jacobian_T << -2 * q2, 2 * q1, 0, 2 * q3, 2 * q0, -4 * q1, -2 * q0, 2 * q3,
        -4 * q2, 2 * q1, 2 * q2, 0;

    gradient_ = jacobian_T * f.ToEigenVector();
    Normalize(gradient_);
    gradient_ *= beta_;
  }
}

}  // namespace drivers
}  // namespace felicia