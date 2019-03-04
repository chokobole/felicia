#ifndef FELICIA_CORE_UTIL_UUID_GENERATOR_H_
#define FELICIA_CORE_UTIL_UUID_GENERATOR_H_

#include "third_party/chromium/base/containers/flat_map.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

template <typename T, typename Traits>
class EXPORT Generator {
 public:
  static constexpr int kMaximumTrial = 1000;

  constexpr Generator() = default;

  T Generate() {
    int trial = 0;
    T value;
    do {
      value = Traits::Generate();
      trial++;
    } while (In(value) && trial < kMaximumTrial);

    if (trial == kMaximumTrial) return Traits::InvalidValue();

    Add(value);
    return value;
  }

  void Return(const T& value) { Remove(value); }

  bool In(const T& value) { return pool_.find(value) != pool_.end(); }

  void Add(const T& t) { pool_.insert_or_assign(t, true); }

  void Remove(const T& t) {
    DCHECK(In(t));
    pool_.erase(pool_.find(t));
  }

 private:
  ::base::flat_map<T, bool> pool_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_UTIL_UUID_GENERATOR_H_