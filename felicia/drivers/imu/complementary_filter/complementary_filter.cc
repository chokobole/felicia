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

ComplementaryFilter::ComplementaryFilter() = default;

void ComplementaryFilter::set_gain_acc(float gain_acc) { gain_acc_ = gain_acc; }

float ComplementaryFilter::gain_acc() const { return gain_acc_; }

void ComplementaryFilter::set_use_adaptive_gain(bool use_adaptive_gain) {
  use_adaptive_gain_ = use_adaptive_gain;
}

bool ComplementaryFilter::use_adaptive_gain() const {
  return use_adaptive_gain_;
}

Quaternionf ComplementaryFilter::orientation() const {
  return orientation_.inverse();
}

void ComplementaryFilter::UpdateAngularVelocity(
    const Vector3f& angular_velocity, base::TimeDelta timestamp) {
  if (!has_measurement_ || last_timestamp_.is_zero()) {
    last_timestamp_ = timestamp;
    return;
  }

  double dt = (timestamp - last_timestamp_).InSecondsF();

  Quaternionf q;
  q.set_vector(angular_velocity * 0.5 * dt);
  q.set_w(0);
  q = orientation_ * q;

  orientation_ += q;
  orientation_.NormalizeInPlace();

  last_timestamp_ = timestamp;
}

void ComplementaryFilter::UpdateLinearAcceleration(
    const Vector3f& linear_acceleration) {
  Vector3f a_vec = linear_acceleration.Normalize();

  if (!has_measurement_) {
    if (a_vec.z() >= 0) {
      double X = sqrt((a_vec.z() + 1) * 0.5);
      orientation_.set_xyzw(-a_vec.y() / (2 * X), a_vec.x() / (2 * X), 0, X);
    } else {
      double X = sqrt((1 - a_vec.z()) * 0.5);
      orientation_.set_xyzw(X, 0, a_vec.x() / (2 * X), -a_vec.y() / (2 * X));
    }

    has_measurement_ = true;
  } else {
    // Acceleration reading rotated into the world frame by the inverse
    // predicted quaternion (predicted gravity):
    Quaternionf q(a_vec, 0);
    Quaternionf rotated = orientation_.inverse() * q * orientation_;

    Quaternionf acc_delta;
    double v = sqrt((rotated.z() + 1) * 0.5);
    acc_delta.set_xyzw(-rotated.y() / (2 * v), rotated.x() / (2 * v), 0, v);

    float gain;
    if (use_adaptive_gain_) {
      gain = GetAdaptiveGain(gain_acc_, linear_acceleration);
    } else {
      gain = gain_acc_;
    }

    acc_delta = Interpolate(acc_delta, gain);

    orientation_ = orientation_ * acc_delta;
    orientation_.NormalizeInPlace();
  }
}

float ComplementaryFilter::GetAdaptiveGain(float alpha, const Vector3f& a_vec) {
  double a_mag = a_vec.Norm();
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

Quaternionf ComplementaryFilter::Interpolate(const Quaternionf& q,
                                             float alpha) {
  // ∆qacc = (1 − α)qI + α∆qacc
  Quaternionf identity;
  if (q.w() > 0.9) {
    return identity.Lerp(q, alpha);
  } else {
    return identity.Slerp(q, alpha);
  }
}

}  // namespace felicia