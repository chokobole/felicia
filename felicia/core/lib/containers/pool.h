#ifndef FELICIA_CORE_LIB_CONTAINERS_POOL_H_
#define FELICIA_CORE_LIB_CONTAINERS_POOL_H_

#include <string.h>

#include <limits>
#include <type_traits>
#include <utility>

#include "third_party/chromium/base/compiler_specific.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/macros.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

template <typename T, typename Size>
class EXPORT Pool {
 public:
  static_assert(std::is_unsigned<Size>::value, "Size type should be unsigned");

  // types
  typedef T value_type;
  typedef Size size_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;

  constexpr Pool() {}
  explicit Pool(size_type capacity)
      : buffer_(reinterpret_cast<T*>(new char[sizeof(T) * capacity])),
        capacity_(capacity) {
    DCHECK(capacity > 0);
  }
  ~Pool() { clear(); }

  Pool(Pool&& other)
      : buffer_(other.buffer_),
        capacity_(other.capacity_),
        push_index_(other.push_index_),
        pop_index_(other.pop_index_),
        is_ailve_after_push_index_(other.is_ailve_after_push_index_) {
    other.buffer_ = nullptr;
    other.capacity_ = 0;
    other.push_index_ = 0;
    other.pop_index_ = 0;
    other.is_ailve_after_push_index_ = false;
  }
  void operator=(Pool&& other) {
    clear();
    buffer_ = other.buffer_;
    capacity_ = other.capacity_;
    push_index_ = other.push_index_;
    pop_index_ = other.pop_index_;
    is_ailve_after_push_index_ = other.is_ailve_after_push_index_;

    other.buffer_ = nullptr;
    other.capacity_ = 0;
    other.push_index_ = 0;
    other.pop_index_ = 0;
    other.is_ailve_after_push_index_ = false;
  }

  // Returns the queue capacity
  ALWAYS_INLINE size_type capacity() const { return capacity_; }
  // Returns the number of elements marked alive
  ALWAYS_INLINE size_type size() const {
    if (is_ailve_after_push_index_) {
      return capacity_ - (pop_index_ - push_index_);
    } else {
      return push_index_ - pop_index_;
    }
  }

  bool empty() const { return size() == 0; }
  // Reserve amount of capacity.
  void reserve(size_type capacity);
  // Clear buffer.
  void clear();

  ALWAYS_INLINE reference front();
  ALWAYS_INLINE const_reference front() const;
  ALWAYS_INLINE reference back();
  ALWAYS_INLINE const_reference back() const;

  ALWAYS_INLINE void push(const value_type& v);
  ALWAYS_INLINE void push(value_type&& v);
  ALWAYS_INLINE void pop();

 private:
  ALWAYS_INLINE void inc_push_index() {
    if (is_ailve_after_push_index_ && push_index_ == pop_index_) {
      // Suppose, we have a case below,
      //     (push_index_)
      //  y       e         f  g ....
      //     (pop_index_)
      //
      // And push 'z' this time.
      //
      //     (push_index_)
      //  y       z         f  g ....
      //     (pop_index_)
      //
      // Then the next element to be popped should be 'f'.
      inc_pop_index();
    }

    push_index_ = inc_index(push_index_);
    if (push_index_ == 0) {
      is_ailve_after_push_index_ = true;
    }
  }

  ALWAYS_INLINE void inc_pop_index() {
    pop_index_ = inc_index(pop_index_);
    if (pop_index_ == 0) {
      is_ailve_after_push_index_ = false;
    }
  }

  ALWAYS_INLINE size_type inc_index(size_type index) const {
    if (index == capacity_ - 1) {
      return 0;
    } else {
      return index + 1;
    }
  }

  ALWAYS_INLINE size_type dec_index(size_type index) const {
    if (index == 0) {
      return capacity_ - 1;
    } else {
      return index - 1;
    }
  }

  value_type* buffer_ = nullptr;
  size_type capacity_ = 0;
  size_type pop_index_ = 0;
  size_type push_index_ = 0;
  // When |push_index_| is increased and back to the first position, the
  // elements are still alive after the |push_index_|. This boolean value will
  // be marked on this condition.
  bool is_ailve_after_push_index_ = false;

  DISALLOW_COPY_AND_ASSIGN(Pool);
};

template <typename T, typename Size>
void Pool<T, Size>::clear() {
  while (!empty()) {
    pop();
  }

  delete[] buffer_;
  buffer_ = nullptr;
}

template <typename T, typename Size>
typename Pool<T, Size>::reference Pool<T, Size>::front() {
  return buffer_[pop_index_];
}

template <typename T, typename Size>
typename Pool<T, Size>::const_reference Pool<T, Size>::front() const {
  return buffer_[pop_index_];
}

template <typename T, typename Size>
typename Pool<T, Size>::reference Pool<T, Size>::back() {
  size_type push_index = dec_index(push_index_);
  return buffer_[push_index];
}

template <typename T, typename Size>
typename Pool<T, Size>::const_reference Pool<T, Size>::back() const {
  size_type push_index = dec_index(push_index_);
  return buffer_[push_index];
}

template <typename T, typename Size>
void Pool<T, Size>::reserve(size_type capacity) {
  Pool<T, Size> new_pool(capacity);
  while (!empty()) {
    new_pool.push(std::move(front()));
    pop();
  }
  *this = std::move(new_pool);
}

template <typename T, typename Size>
void Pool<T, Size>::push(const value_type& v) {
  new (&buffer_[push_index_]) T(v);
  inc_push_index();
}

template <typename T, typename Size>
void Pool<T, Size>::push(value_type&& v) {
  new (&buffer_[push_index_]) T(std::move(v));
  inc_push_index();
}

template <typename T, typename Size>
void Pool<T, Size>::pop() {
  if (!is_ailve_after_push_index_ && pop_index_ == push_index_) return;
  buffer_[pop_index_].~T();
  inc_pop_index();
}

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_CONTAINERS_FAST_POOL_H_
