#ifndef FELICIA_CORE_LIB_BASE_CHOICES_H_
#define FELICIA_CORE_LIB_BASE_CHOICES_H_

#include <type_traits>
#include <vector>

#include "felicia/core/lib/base/template_util.h"

namespace felicia {

template <typename T>
class Choices {
 public:
  static_assert(!std::is_same<T, bool>::value,
                "Bool type should not be value_type of Choices");

  template <typename... Rest>
  Choices(Rest... rest) {
    internal::AddValue(values_, (T)rest...);
  }
  Choices(const std::vector<T>& values) : values_(values) {}
  Choices(const Choices& other) = default;
  Choices& operator=(const Choices& other) = default;

  const std::vector<T>& values() const { return values_; }

  bool In(T value) const {
    for (auto& value_ : values_) {
      if (internal::IsEqual(value, value_)) {
        return true;
      }
    }
    return false;
  }

 private:
  std::vector<T> values_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_BASE_CHOICES_H_