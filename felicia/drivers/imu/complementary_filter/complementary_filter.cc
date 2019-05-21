/*
  @author Roberto G. Valenti <robertogl.valenti@gmail.com>

        @section LICENSE
  Copyright (c) 2015, City University of New York
  CCNY Robotics Lab <http://robotics.ccny.cuny.edu>
        All rights reserved.

        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions are
  met:
     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
     3. Neither the name of the City College of New York nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
        DISCLAIMED. IN NO EVENT SHALL the CCNY ROBOTICS LAB BE LIABLE FOR ANY
        DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "felicia/drivers/imu/complementary_filter/complementary_filter.h"

namespace felicia {

constexpr double kGravity = 9.81;

void ComplementaryFilter::set_gain_acc(float gain_acc) { gain_acc_ = gain_acc; }

float ComplementaryFilter::gain_acc() const { return gain_acc_; }

void ComplementaryFilter::set_use_adaptive_gain(bool use_adaptive_gain) {
  use_adaptive_gain_ = use_adaptive_gain;
}

bool ComplementaryFilter::use_adaptive_gain() const {
  return use_adaptive_gain_;
}

::Eigen::Quaternionf ComplementaryFilter::orientation() const {
  return orientation_.inverse();
}

void ComplementaryFilter::UpdateAngularVelocity(float x, float y, float z,
                                                ::base::TimeDelta timestamp) {
  if (!has_measurement_ || last_timestamp_.is_zero()) {
    last_timestamp_ = timestamp;
    return;
  }

  double dt = (timestamp - last_timestamp_).InSecondsF();

  ::Eigen::Quaternionf q;
  q.w() = 0;
  q.vec() = ::Eigen::Vector3f(x, y, z) * 0.5 * dt * -1;

  q = q * orientation_;

  orientation_.vec() += q.vec();
  orientation_.w() += q.w();
  orientation_.normalize();

  last_timestamp_ = timestamp;
}

void ComplementaryFilter::UpdateLinearAcceleration(float x, float y, float z) {
  ::Eigen::Vector3f v;
  v << x, y, z;
  v.normalize();

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
    // Acceleration reading rotated into the world frame by the inverse
    // predicted quaternion (predicted gravity):
    ::Eigen::Quaternionf q;
    q.vec() = v;
    ::Eigen::Quaternionf rotated = orientation_.inverse() * q * orientation_;

    ::Eigen::Quaternionf acc_delta;
    double v = sqrt((rotated.z() + 1) * 0.5);
    acc_delta.w() = v;
    acc_delta.x() = -rotated.y() / (2 * v);
    acc_delta.y() = rotated.x() / (2 * v);
    acc_delta.z() = 0;

    float gain;
    if (use_adaptive_gain_) {
      gain = GetAdaptiveGain(gain_acc_, x, y, z);
    } else {
      gain = gain_acc_;
    }

    Interpolate(acc_delta, gain);

    orientation_ = orientation_ * acc_delta;
    orientation_.normalize();
  }
}

float ComplementaryFilter::GetAdaptiveGain(float alpha, float ax, float ay,
                                           float az) {
  double a_mag = sqrt(ax * ax + ay * ay + az * az);
  double error = fabs(a_mag - kGravity) / kGravity;
  constexpr double error1 = 0.1;
  constexpr double error2 = 0.2;
  constexpr double m = 1.0 / (error1 - error2);
  constexpr double b = 1.0 - m * error1;
  double factor;
  if (error < error1)
    factor = 1.0;
  else if (error < error2)
    factor = m * error + b;
  else
    factor = 0.0;
  return factor * alpha;
}

void ComplementaryFilter::Interpolate(::Eigen::Quaternionf& q, float alpha) {
  // ∆qacc = (1 − α)qI + α∆qacc
  if (q.w() > 0.9) {
    // Lerp (Linear interpolation):
    q.w() = (1 - alpha) + alpha * q.w();
    q.vec() *= alpha;
  } else {
    // Slerp (Spherical linear interpolation):
    double angle = acos(q.w());
    double A = sin(angle * (1.0 - alpha)) / sin(angle);
    double B = sin(angle * alpha) / sin(angle);
    q.w() = A + B * q.w();
    q.vec() *= B;
  }
}

}  // namespace felicia