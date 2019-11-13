// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_UNIT_UNIT_HELPER_H_
#define FELICIA_CORE_LIB_UNIT_UNIT_HELPER_H_

#include <limits>

#include "third_party/chromium/base/numerics/safe_math.h"

namespace felicia {
namespace internal {

template <typename T>
T SaturateAdd(T value, T value2) {
  base::CheckedNumeric<T> rv(value);
  rv += value2;
  if (rv.IsValid()) return rv.ValueOrDie();
  // Positive RHS overflows. Negative RHS underflows.
  if (value2 < 0) return std::numeric_limits<T>::min();
  return std::numeric_limits<T>::max();
}

template <typename T>
T SaturateSub(T value, T value2) {
  base::CheckedNumeric<T> rv(value);
  rv -= value2;
  if (rv.IsValid()) return rv.ValueOrDie();
  // Negative RHS overflows. Positive RHS underflows.
  if (value2 < 0) return std::numeric_limits<T>::max();
  return std::numeric_limits<T>::min();
}

template <typename T, typename U>
T SaturateMul(T value, U value2) {
  base::CheckedNumeric<T> rv(value);
  rv *= value2;
  if (rv.IsValid()) return rv.ValueOrDie();
  // Matched sign overflows. Mismatched sign underflows.
  if ((value < 0) ^ (value2 < 0)) return std::numeric_limits<T>::min();
  return std::numeric_limits<T>::max();
}

template <typename T, typename U>
T SaturateDiv(T value, U value2) {
  base::CheckedNumeric<T> rv(value);
  rv /= value2;
  if (rv.IsValid()) return rv.ValueOrDie();
  // Matched sign overflows. Mismatched sign underflows.
  if ((value < 0) ^ (value2 < 0)) return std::numeric_limits<T>::min();
  return std::numeric_limits<T>::max();
}

template <typename T>
T FromProduct(T value, T positive_multiplier) {
  DCHECK(positive_multiplier > 0);
  return value > std::numeric_limits<int64_t>::max() / positive_multiplier
             ? std::numeric_limits<int64_t>::max()
             : value < std::numeric_limits<int64_t>::min() / positive_multiplier
                   ? std::numeric_limits<int64_t>::min()
                   : value * positive_multiplier;
}

}  // namespace internal
}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_UNIT_HELPER_H_