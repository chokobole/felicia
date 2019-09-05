#ifndef FELICIA_CORE_LIB_CONTAINERS_POOL_H_
#define FELICIA_CORE_LIB_CONTAINERS_POOL_H_

#include <string.h>

#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>

#include "third_party/chromium/base/compiler_specific.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/stringprintf.h"

namespace felicia {

template <typename SizeType>
struct PoolIndex {
  static_assert(std::is_unsigned<SizeType>::value,
                "Size type should be unsigned");

  typedef SizeType size_type;

  PoolIndex() = default;
  explicit PoolIndex(size_type capacity) : capacity(capacity) {}
  PoolIndex(size_type capacity, size_type pop_index)
      : capacity(capacity), pop_index(pop_index) {}
  PoolIndex(const PoolIndex& other) = default;
  PoolIndex& operator=(const PoolIndex& other) = default;

  ALWAYS_INLINE void inc_push_index() {
    push_index = inc_index(push_index);
    if (push_index == 0) {
      is_ailve_after_push_index = true;
    }
  }

  ALWAYS_INLINE void inc_pop_index() {
    pop_index = inc_index(pop_index);
    if (pop_index == 0) {
      is_ailve_after_push_index = false;
    }
  }

  ALWAYS_INLINE size_type inc_index(size_type index) const {
    if (index == capacity - 1) {
      return 0;
    } else {
      return index + 1;
    }
  }

  ALWAYS_INLINE size_type inc_index(size_type index, size_type delta) const {
    return (index + delta) % capacity;
  }

  ALWAYS_INLINE size_type dec_index(size_type index) const {
    if (index == 0) {
      return capacity - 1;
    } else {
      return index - 1;
    }
  }

  size_type size() const {
    if (is_ailve_after_push_index) {
      return capacity - (pop_index - push_index);
    } else {
      return push_index - pop_index;
    }
  }

  void reset() {
    capacity = 0;
    push_index = 0;
    pop_index = 0;
    is_ailve_after_push_index = false;
  }

  bool operator==(const PoolIndex& other) const {
    return capacity == other.capacity && pop_index == other.pop_index &&
           push_index == other.push_index &&
           is_ailve_after_push_index == other.is_ailve_after_push_index;
  }
  bool operator!=(const PoolIndex& other) const { return !operator==(other); }

  size_type capacity = 0;
  size_type pop_index = 0;
  size_type push_index = 0;
  // When |push_index| is increased and back to the first position, the
  // elements are still alive after the |push_index|. This boolean value will
  // be marked on this condition.
  bool is_ailve_after_push_index = false;
};

template <typename T, typename SizeType>
class Pool {
 public:
  static_assert(std::is_unsigned<SizeType>::value,
                "Size type should be unsigned");

  class ConstIterator;
  class Iterator {
   public:
    typedef typename std::iterator_traits<T*>::value_type value_type;
    typedef SizeType size_type;
    typedef size_type difference_type;
    typedef T* pointer;
    typedef T& reference;
    typedef std::forward_iterator_tag iterator_category;

    friend class Pool<T, SizeType>::ConstIterator;

    enum IteratorEnd { kEnd };

    Iterator() = default;
    Iterator(const Pool& pool)
        : pool_index_(pool.pool_index_), buffer_(pool.buffer_) {}
    Iterator(const Pool& pool, IteratorEnd kEnd)
        : pool_index_(pool.pool_index_), buffer_(pool.buffer_) {
      pool_index_.pop_index = pool_index_.push_index;
      pool_index_.is_ailve_after_push_index = false;
    }

    Iterator(const Iterator& other) = default;
    ~Iterator() = default;

    Iterator& operator=(const Iterator& other) = default;

    bool operator==(const Iterator& other) const {
      return pool_index_ == other.pool_index_;
    }

    bool operator!=(const Iterator& other) const {
      return pool_index_ != other.pool_index_;
    }

    Iterator& operator++() {
      CHECK_GT(pool_index_.size(), 0);
      pool_index_.inc_pop_index();
      return *this;
    }

    Iterator operator++(int) {
      Iterator old = *this;
      ++*this;
      return old;
    }

    reference operator*() const {
      CHECK_GT(pool_index_.size(), 0);
      return buffer_[pool_index_.pop_index];
    }

    pointer operator->() const {
      CHECK_GT(pool_index_.size(), 0);
      return buffer_[pool_index_.pop_index];
    }

   private:
    PoolIndex<SizeType> pool_index_;
    T* buffer_;
  };

  class ConstIterator {
   public:
    typedef typename std::iterator_traits<T*>::value_type value_type;
    typedef SizeType size_type;
    typedef size_type difference_type;
    typedef const T* pointer;
    typedef const T& reference;
    typedef std::forward_iterator_tag iterator_category;

    enum ConstIteratorEnd { kEnd };

    ConstIterator() = default;
    ConstIterator(const Pool& pool)
        : pool_index_(pool.pool_index_), buffer_(pool.buffer_) {}
    ConstIterator(const Pool& pool, ConstIteratorEnd kEnd)
        : pool_index_(pool.pool_index_), buffer_(pool.buffer_) {
      pool_index_.pop_index = pool_index_.push_index;
      pool_index_.is_ailve_after_push_index = false;
    }
    ConstIterator(const Iterator& other)
        : pool_index_(other.pool_index_), buffer_(other.buffer_) {}

    ConstIterator(const ConstIterator& other) = default;
    ~ConstIterator() = default;

    ConstIterator& operator=(const ConstIterator& other) = default;

    bool operator==(const ConstIterator& other) const {
      return pool_index_ == other.pool_index_;
    }

    bool operator!=(const ConstIterator& other) const {
      return pool_index_ != other.pool_index_;
    }

    ConstIterator& operator++() {
      CHECK_GT(pool_index_.size(), 0);
      pool_index_.inc_pop_index();
      return *this;
    }

    ConstIterator operator++(int) {
      ConstIterator old = *this;
      ++*this;
      return old;
    }

    ConstIterator operator+(size_type delta) const {
      ConstIterator ret = *this;
      ret.pool_index_.inc_index(pool_index_.pop_index, delta);
      return ret;
    }

    reference operator*() const {
      CHECK_GT(pool_index_.size(), 0);
      return buffer_[pool_index_.pop_index];
    }

    pointer operator->() const {
      CHECK_GT(pool_index_.size(), 0);
      return buffer_[pool_index_.pop_index];
    }

   private:
    PoolIndex<SizeType> pool_index_;
    T* buffer_;
  };

  // types
  typedef T value_type;
  typedef SizeType size_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef Iterator iterator;
  typedef ConstIterator const_iterator;

  constexpr Pool() {}
  explicit Pool(size_type capacity)
      : buffer_(reinterpret_cast<T*>(new char[sizeof(T) * capacity])),
        pool_index_(capacity) {
    DCHECK(capacity > 0);
  }
  ~Pool() { clear(); }

  // Returns the queue capacity
  ALWAYS_INLINE size_type capacity() const { return pool_index_.capacity; }
  // Returns the number of elements marked alive
  ALWAYS_INLINE size_type size() const { return pool_index_.size(); }

  bool empty() const { return size() == 0; }
  // Set the amount of capacity reserving elements inside at most.
  void reserve(size_type capacity);
  // Clear buffer.
  void clear();

  ALWAYS_INLINE reference front() { return buffer_[pool_index_.pop_index]; }

  ALWAYS_INLINE const_reference front() const {
    return buffer_[pool_index_.pop_index];
  }

  ALWAYS_INLINE reference back() {
    return buffer_[pool_index_.dec_index(pool_index_.push_index)];
  }

  ALWAYS_INLINE const_reference back() const {
    return buffer_[pool_index_.dec_index(pool_index_.push_index)];
  }

  ALWAYS_INLINE reference at(size_type idx) {
    range_check(idx);
    return (*this)[idx];
  }

  ALWAYS_INLINE const_reference at(size_type idx) const {
    range_check(idx);
    return (*this)[idx];
  }

  ALWAYS_INLINE reference operator[](size_type idx) { return *(begin() + idx); }

  ALWAYS_INLINE const_reference operator[](size_type idx) const {
    return *(cbegin() + idx);
  }

  ALWAYS_INLINE void push(const value_type& v);
  ALWAYS_INLINE void push(value_type&& v);
  template <typename... Args>
  ALWAYS_INLINE void emplace(Args&&... args);
  ALWAYS_INLINE void pop();

  iterator begin() const noexcept { return iterator(*this); }
  const_iterator cbegin() const noexcept { return begin(); }

  iterator end() const noexcept { return iterator(*this, iterator::kEnd); }
  const_iterator cend() const noexcept { return end(); }

 private:
  void range_check(size_type idx) const {
    if (idx >= size())
      throw std::out_of_range(base::StringPrintf(
          "Pool::range_check: (%zd >= %zd)", static_cast<size_t>(idx),
          static_cast<size_t>(size())));
  }

  friend class Iterator;

  value_type* buffer_ = nullptr;
  PoolIndex<SizeType> pool_index_;

  DISALLOW_COPY_AND_ASSIGN(Pool);
};

template <typename T, typename SizeType>
void Pool<T, SizeType>::clear() {
  while (!empty()) {
    pop();
  }

  delete[] reinterpret_cast<char*>(buffer_);
  buffer_ = nullptr;
  pool_index_.reset();
}

template <typename T, typename SizeType>
void Pool<T, SizeType>::reserve(size_type capacity) {
  Pool<T, SizeType> new_pool(capacity);
  while (!empty()) {
    new_pool.push(std::move(front()));
    pop();
  }

  delete[] reinterpret_cast<char*>(buffer_);

  buffer_ = new_pool.buffer_;
  pool_index_ = new_pool.pool_index_;

  new_pool.buffer_ = nullptr;
  new_pool.pool_index_.reset();
}

template <typename T, typename SizeType>
void Pool<T, SizeType>::push(const value_type& v) {
  DCHECK(pool_index_.capacity > 0);
  if (pool_index_.is_ailve_after_push_index) pop();
  new (&buffer_[pool_index_.push_index]) T(v);
  pool_index_.inc_push_index();
}

template <typename T, typename SizeType>
void Pool<T, SizeType>::push(value_type&& v) {
  DCHECK(pool_index_.capacity > 0);
  if (pool_index_.is_ailve_after_push_index) pop();
  new (&buffer_[pool_index_.push_index]) T(std::move(v));
  pool_index_.inc_push_index();
}

template <typename T, typename SizeType>
template <typename... Args>
void Pool<T, SizeType>::emplace(Args&&... args) {
  DCHECK(pool_index_.capacity > 0);
  if (pool_index_.is_ailve_after_push_index) pop();
  new (&buffer_[pool_index_.push_index]) T(std::forward<Args>(args)...);
  pool_index_.inc_push_index();
}

template <typename T, typename SizeType>
void Pool<T, SizeType>::pop() {
  DCHECK(pool_index_.capacity > 0);
  if (empty()) return;
  buffer_[pool_index_.pop_index].~T();
  pool_index_.inc_pop_index();
}

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_CONTAINERS_FAST_POOL_H_
