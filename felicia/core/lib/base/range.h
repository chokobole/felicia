// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_BASE_RANGE_H_
#define FELICIA_CORE_LIB_BASE_RANGE_H_

#include <sstream>

#include "felicia/core/lib/base/template_util.h"

namespace felicia {

template <typename T,
          std::enable_if_t<internal::SupportsLessThanOrEqualOperator<T>::value,
                           void*> = nullptr>
class Range {
 public:
  static_assert(!std::is_same<T, bool>::value,
                "Bool type should not be value_type of Range");

  Range(T from, T to) : from_(from), to_(to) {}
  Range(const Range& other) = default;
  Range& operator=(const Range& other) = default;

  const T from() const { return from_; }
  const T to() const { return to_; }

  bool In(T value) const { return from_ <= value && value <= to_; }

 private:
  T from_;
  T to_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_BASE_RANGE_H_