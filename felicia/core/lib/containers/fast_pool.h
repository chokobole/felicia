#ifndef FELICIA_CORE_LIB_CONTAINERS_FAST_POOL_H_
#define FELICIA_CORE_LIB_CONTAINERS_FAST_POOL_H_

#include <string.h>

#include <limits>

#include "absl/meta/type_traits.h"

#include "felicia/core/lib/base/compiler_specific.h"

namespace felicia {

/*
FastPool is a container which has fixed small size, features fast pushing and
popping. It only can be resized at intialization.
*/
template <typename T, typename Size>
class FastPool {
 public:
  static_assert(std::is_integral<Size>::value, "Size type should be integral");
  static_assert(sizeof(Size) <= 2,
                "size of Size type should be less than or equal to 2");

  // types
  typedef T value_type;
  typedef Size size_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;

  FastPool()
      : buffer_(nullptr),
        size_(0),
        pop_index_(max_size()),
        push_index_(0),
        is_ailve_after_push_index_(false) {}
  FastPool(size_type size) : FastPool() { resize(size); }
  ~FastPool() { delete[] buffer_; }

  // Returns the queue size
  ALWAYS_INLINE size_type size() const { return size_; }
  // Returns the length of elements marked alive
  ALWAYS_INLINE size_type length() const {
    if (is_ailve_after_push_index_) {
      return size_ - (pop_index_ - push_index_);
    } else {
      return push_index_ - pop_index_;
    }
  }

  void clear();

  ALWAYS_INLINE reference front();
  ALWAYS_INLINE const_reference front() const;
  ALWAYS_INLINE reference back();
  ALWAYS_INLINE const_reference back() const;

  ALWAYS_INLINE void push(const value_type& v);
  ALWAYS_INLINE void push(value_type&& v);
  ALWAYS_INLINE void pop();

 private:
  ALWAYS_INLINE size_type max_size() const {
    return std::numeric_limits<size_type>::max();
  }
  ALWAYS_INLINE void inc_push_index() {
    if (is_ailve_after_push_index_ && push_index_ == pop_index_) {
      // When pushed element somewhere supposed to be popped, then |pop_index_|
      // should be moved.
      inc_pop_index();
    }

    push_index_++;
    if (PREDICT_FALSE(push_index_ == size_)) {
      push_index_ = 0;
      is_ailve_after_push_index_ = true;
    }

    if (PREDICT_FALSE(pop_index_ == max_size())) {
      // When pushed element for the first time, then |pop_index_| is set to 0.
      pop_index_ = 0;
    }
  }

  ALWAYS_INLINE void inc_pop_index() {
    pop_index_++;
    if (PREDICT_FALSE(pop_index_ == size_)) {
      pop_index_ = 0;
      is_ailve_after_push_index_ = false;
    }
  }

  // Resize amount of size. this should be called at initialization.
  void resize(size_type size);

  value_type* buffer_;
  size_type size_;
  size_type pop_index_;
  size_type push_index_;
  // When |push_index_| is increased and back to the first position, the
  // elements are still alive after the |push_index_|. This boolean value will
  // be marked on this condition.
  bool is_ailve_after_push_index_;
};

template <typename T, typename Size>
void FastPool<T, Size>::clear() {
  delete[] buffer_;
  buffer_ = nullptr;
}

template <typename T, typename Size>
typename FastPool<T, Size>::reference FastPool<T, Size>::front() {
  return buffer_[pop_index_];
}

template <typename T, typename Size>
typename FastPool<T, Size>::const_reference FastPool<T, Size>::front() const {
  return buffer_[pop_index_];
}

template <typename T, typename Size>
typename FastPool<T, Size>::reference FastPool<T, Size>::back() {
  size_type push_index = push_index_ == 0 ? size_ - 1 : push_index_ - 1;
  return buffer_[push_index];
}

template <typename T, typename Size>
typename FastPool<T, Size>::const_reference FastPool<T, Size>::back() const {
  size_type push_index = push_index_ == 0 ? size_ - 1 : push_index_ - 1;
  return buffer_[push_index];
}

template <typename T, typename Size>
void FastPool<T, Size>::resize(size_type size) {
  buffer_ = new value_type[size];
  size_ = size;
}

template <typename T, typename Size>
void FastPool<T, Size>::push(const value_type& v) {
  buffer_[push_index_] = v;
  inc_push_index();
}

template <typename T, typename Size>
void FastPool<T, Size>::push(value_type&& v) {
  buffer_[push_index_] = std::move(v);
  inc_push_index();
}

template <typename T, typename Size>
void FastPool<T, Size>::pop() {
  if (!is_ailve_after_push_index_ && pop_index_ == push_index_) return;
  inc_pop_index();
}

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_CONTAINERS_FAST_POOL_H_
