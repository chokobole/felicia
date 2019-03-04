#ifndef FELICIA_CORE_UTIL_UUID_GENERATOR_H_
#define FELICIA_CORE_UTIL_UUID_GENERATOR_H_

#include "third_party/chromium/base/containers/flat_map.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

template <typename T, typename Traits>
class EXPORT Generator {
 public:
  constexpr Generator() = default;

  T Generate() {
    T value = Traits::Generate();
    this->Add(value);
    return value;
  }

  void Return(const T& value) { this->Remove(value); }

  bool In(const T& value) { return pool_.find(value) != pool_.end(); }

 protected:
  void Add(const T& t) { pool_.insert_or_assign(t, true); }

  void Remove(const T& t) { pool_.erase(pool_.find(t)); }

  ::base::flat_map<T, bool> pool_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_UTIL_UUID_GENERATOR_H_