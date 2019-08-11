#ifndef FELICIA_CORE_LIB_BASE_TEMPLATE_UTIL_H_
#define FELICIA_CORE_LIB_BASE_TEMPLATE_UTIL_H_

#include <string>
#include <type_traits>
#include <utility>
#include <vector>

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

template <typename T,
          std::enable_if_t<internal::SupportsEqualOperator<T>::value &&
                               !std::is_same<T, std::string>::value,
                           void*> = nullptr>
bool IsEqual(const T& v1, const T& v2) {
  return v1 == v2;
}

template <typename T, std::enable_if_t<std::is_same<T, std::string>::value,
                                       void*> = nullptr>
bool IsEqual(const T& v1, const T& v2) {
  return v1 == v2;
}

template <typename T>
void AddValue(std::vector<T>& vec, T&& value) {
  vec.push_back(std::forward<T>(value));
}

template <typename T, typename... Rest>
void AddValue(std::vector<T>& vec, T&& value, Rest&&... rest) {
  vec.push_back(std::forward<T>(value));
  AddValue(vec, std::forward<Rest>(rest)...);
}

}  // namespace internal
}  // namespace felicia

#endif