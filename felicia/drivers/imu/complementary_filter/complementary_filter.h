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

#ifndef FELICIA_DRIVERS_IMU_COMPLEMENTARY_FILTER_COMPLEMENTARY_FILTER_H_
#define FELICIA_DRIVERS_IMU_COMPLEMENTARY_FILTER_COMPLEMENTARY_FILTER_H_

#include "felicia/drivers/imu/imu_filter_interface.h"

namespace felicia {
namespace drivers {

class ComplementaryFilter : public ImuFilterInterface {
 public:
  void set_gain_acc(float gain_acc);
  float gain_acc() const;
  void set_use_adaptive_gain(bool use_adaptive_gain);
  bool use_adaptive_gain() const;

  Quaternionf orientation() const override;
  void UpdateAngularVelocity(const Vector3f& angular_velocity,
                             base::TimeDelta timestamp) override;
  void UpdateLinearAcceleration(const Vector3f& linear_acceleration) override;

 private:
  friend class ImuFilterFactory;

  ComplementaryFilter();

  float GetAdaptiveGain(float alpha, const Vector3f& a_vec);
  Quaternionf Interpolate(const Quaternionf& q, float alpha);

  float gain_acc_ = 0.01;
  bool use_adaptive_gain_ = false;

  bool has_measurement_ = false;
  base::TimeDelta last_timestamp_;
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_COMPLEMENTARY_FILTER_COMPLEMENTARY_FILTER_H_