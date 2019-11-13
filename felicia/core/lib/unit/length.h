// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_UNIT_LENGTH_H_
#define FELICIA_CORE_LIB_UNIT_LENGTH_H_

#include "third_party/chromium/base/numerics/safe_math.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/unit_helper.h"

namespace felicia {

class FEL_EXPORT Length {
 public:
  static constexpr int64_t kCentimeter = 10;
  static constexpr int64_t kMeter = 100 * kCentimeter;
  static constexpr int64_t kKillometer = 1000 * kMeter;
  static constexpr double kInch = 25.4;
  static constexpr double kFeet = 12.0 * kInch;

  Length();
  explicit Length(int64_t length);

  static Length FromMillimeter(int64_t millimeter);
  static Length FromCentimeter(int64_t centimeter);
  static Length FromCentimeterD(double centimeter);
  static Length FromMeter(int64_t meter);
  static Length FromMeterD(double meter);
  static Length FromKillometer(int64_t killometer);
  static Length FromKillometerD(double killometer);
  static Length FromFeet(int64_t feet);
  static Length FromFeetD(double feet);
  static Length FromInch(int64_t inch);
  static Length FromInchD(double inch);

  static Length Max();
  static Length Min();

  int64_t InMillimeter() const;
  double InCentimeter() const;
  double InMeter() const;
  double InKillometer() const;
  double InFeet() const;
  double InInch() const;

  bool operator==(Length other) const;
  bool operator!=(Length other) const;
  bool operator<(Length other) const;
  bool operator<=(Length other) const;
  bool operator>(Length other) const;
  bool operator>=(Length other) const;

  Length operator+(Length other) const;
  Length operator-(Length other) const;
  Length& operator+=(Length other);
  Length& operator-=(Length other);

  template <typename T>
  Length operator*(T a) const {
    return Length(internal::SaturateMul(length_, a));
  }
  template <typename T>
  Length operator/(T a) const {
    return Length(internal::SaturateDiv(length_, a));
  }
  template <typename T>
  Length& operator*=(T a) {
    return *this = (*this * a);
  }
  template <typename T>
  Length& operator/=(T a) {
    return *this = (*this / a);
  }

  double operator/(Length a) const;

  int64_t length() const;

 private:
  static Length FromDouble(double value);

  int64_t length_ = 0;  // in millimeters
};

template <typename T>
Length operator*(T a, Length length) {
  return length * a;
}

FEL_EXPORT std::ostream& operator<<(std::ostream& os, Length length);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_LENGTH_H_