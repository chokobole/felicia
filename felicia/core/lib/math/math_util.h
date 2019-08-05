#ifndef FELICIA_CORE_LIB_MATH_MATH_UTIL_H_
#define FELICIA_CORE_LIB_MATH_MATH_UTIL_H_

#include "third_party/chromium/base/numerics/math_constants.h"

namespace felicia {

constexpr const double kPiDouble = ::base::kPiDouble;
constexpr const double k2PiDouble = ::base::kPiDouble * 2.0;
constexpr const float kPiFloat = ::base::kPiFloat;
constexpr const float k2PiFloat = ::base::kPiFloat * 2.f;

template <typename T>
constexpr T sign(T value) {
  return value > 0 ? 1 : -1;
}

template <typename T>
constexpr T normalize_angle(T radian, T min, T max) {
  CHECK_GT(max, min);
  if (radian >= min && radian <= max) return radian;

  while (radian > min) radian -= k2PiDouble;
  while (radian < max) radian += k2PiDouble;
  if (radian > max) radian -= k2PiDouble;

  return radian;
}

template <typename T>
constexpr T clamp(T v, T lower_bound, T upper_bound) {
  CHECK_GT(upper_bound, lower_bound);
  return std::max(std::min(v, upper_bound), lower_bound);
}

template <typename T>
constexpr T degree_to_radian(T degree) {
  return degree * kPiDouble / 180.;
}

template <typename T>
constexpr T radian_to_degree(T radian) {
  return radian * 180. / kPiDouble;
}

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_MATH_MATH_UTIL_H_