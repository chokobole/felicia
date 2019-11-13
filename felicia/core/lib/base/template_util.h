// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_BASE_TEMPLATE_UTIL_H_
#define FELICIA_CORE_LIB_BASE_TEMPLATE_UTIL_H_

#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "third_party/chromium/base/bind.h"

namespace felicia {
namespace internal {

// Uses expression SFINAE to detect whether using operator== would work.
template <typename, typename = void>
struct SupportsEqualOperator : std::false_type {};
template <typename T>
struct SupportsEqualOperator<T, decltype(void(std::declval<T>() ==
                                              std::declval<T>()))>
    : std::true_type {};

// Uses expression SFINAE to detect whether using operator<= would work.
template <typename, typename = void>
struct SupportsLessThanOrEqualOperator : std::false_type {};
template <typename T>
struct SupportsLessThanOrEqualOperator<T, decltype(void(std::declval<T>() <=
                                                        std::declval<T>()))>
    : std::true_type {};

// Uses expression SFINAE to detect whether using operator+ would work.
template <typename, typename = void>
struct SupportsPlusOperator : std::false_type {};
template <typename T>
struct SupportsPlusOperator<T, decltype(
                                   void(std::declval<T>() + std::declval<T>()))>
    : std::true_type {};

template <typename T>
void AddValue(std::vector<T>& vec, T&& value) {
  vec.push_back(std::forward<T>(value));
}

template <typename T, typename... Rest>
void AddValue(std::vector<T>& vec, T&& value, Rest&&... rest) {
  vec.push_back(std::forward<T>(value));
  AddValue(vec, std::forward<Rest>(rest)...);
}

template <size_t n, typename List>
struct PickTypeListItemImpl;

template <size_t n, typename T, typename... List>
struct PickTypeListItemImpl<n, base::internal::TypeList<T, List...>>
    : PickTypeListItemImpl<n - 1, base::internal::TypeList<List...>> {};

template <typename T, typename... List>
struct PickTypeListItemImpl<0, base::internal::TypeList<T, List...>> {
  using RawType = T;
  using Type = std::decay_t<T>;
};

template <size_t n, typename List>
using PickTypeListItem = typename PickTypeListItemImpl<n, List>::Type;

}  // namespace internal
}  // namespace felicia

#endif