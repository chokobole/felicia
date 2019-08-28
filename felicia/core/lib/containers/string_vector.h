#ifndef FELICIA_CORE_LIB_CONTAINERS_STRING_VECTOR_H_
#define FELICIA_CORE_LIB_CONTAINERS_STRING_VECTOR_H_

#include <string>
#include <type_traits>

#include "third_party/chromium/base/containers/checked_iterators.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

// StringVector is a container which has std::string inside, but is able to
// be used like std::vector. It is used as a c++ implementation against
// bytes type of protobuf. For example, if you want to use protobuf bytes type,
// but in c++ it can be float or int, you can use this class like below.
//
// StringVector data;
// StringVector::View<int> view = data.AsView<int>();
// or
// StringVector::View<float> view = data.AsView<float>();
class EXPORT StringVector {
 public:
  template <typename T>
  class ConstView {
   public:
    static_assert(std::is_trivially_destructible<T>::value,
                  "Data type needs to be trivially_destructible.");

    using const_iterator = base::CheckedRandomAccessConstIterator<T>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    explicit ConstView(const StringVector& vector) : vector_(vector) {}

    const_iterator cbegin() const noexcept {
      return make_iterator(const_cast<T*>(data()));
    }

    const_iterator cend() const noexcept {
      return make_iterator(const_cast<T*>(data() + size()));
    }

    const_reverse_iterator crbegin() const noexcept {
      return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const noexcept {
      return const_reverse_iterator(cbegin());
    }

    const T* data() const {
      return reinterpret_cast<const T*>(vector_.data_.data());
    }

    const T& operator[](size_t idx) const { return *(cbegin() + idx); }

    const T& at(size_t idx) const {
      range_check(idx);
      return (*this)[idx];
    }

    const T& front() const { return *cbegin(); }
    const T& back() const { return *(cend() - 1); }

    size_t size() const noexcept { return vector_.size() / sizeof(T); }

    bool empty() const noexcept { return vector_.size() == 0; }

   private:
    void range_check(size_t idx) const {
      if (idx >= size())
        throw std::out_of_range(base::StringPrintf(
            "StringVector::range_check: (%zd >= %zd)", idx, size()));
    }

    const_iterator make_iterator(T* ptr) const {
      return const_iterator(const_cast<T*>(data()), ptr,
                            const_cast<T*>(data() + size()));
    }

    const StringVector& vector_;
  };

  template <typename T>
  class View {
   public:
    static_assert(std::is_trivially_destructible<T>::value,
                  "Data type needs to be trivially_destructible.");

    using iterator = base::CheckedRandomAccessIterator<T>;
    using const_iterator = base::CheckedRandomAccessConstIterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    explicit View(StringVector& vector) : vector_(vector) {}

    void push_back(const T& value) {
      T* ptr = AddOneElementToBack();
      new (ptr) T(value);
    }

    void push_back(T&& value) {
      T* ptr = AddOneElementToBack();
      new (ptr) T(std::move(value));
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
      T* ptr = AddOneElementToBack();
      new (ptr) T(std::forward<Args>(args)...);
    }

    void pop_back() {
      back().~T();
      vector_.resize(vector_.size() - sizeof(T));
    }

    iterator insert(const_iterator position, const T& value) {
      T* ptr = AddElements(position, 1);
      new (ptr) T(value);
      return make_iterator(ptr);
    }

    iterator insert(const_iterator position, T&& value) {
      T* ptr = AddElements(position, 1);
      new (ptr) T(std::move(value));
      return make_iterator(ptr);
    }

    void insert(const_iterator position, size_t n, const T& value) {
      T* ptr = AddElements(position, n);
      for (size_t i = 0; i < n; ++i) {
        new (ptr + i) T(value);
      }
    }

    iterator erase(const_iterator position) {
      ptrdiff_t diff = position - begin();
      vector_.data_.erase(diff * sizeof(T), sizeof(T));
      return begin() + diff;
    }

    iterator erase(const_iterator first, const_iterator last) {
      ptrdiff_t diff = first - begin();
      vector_.data_.erase(diff * sizeof(T), (last - first) * sizeof(T));
      return begin() + diff;
    }

    iterator begin() const noexcept {
      return make_iterator(const_cast<T*>(data()));
    }
    const_iterator cbegin() const noexcept { return begin(); }

    iterator end() const noexcept {
      return make_iterator(const_cast<T*>(data() + size()));
    }
    const_iterator cend() const noexcept { return end(); }

    reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }

    reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return rend(); }

    const T* data() const {
      return reinterpret_cast<const T*>(vector_.data_.data());
    }
    T* data() {
      return reinterpret_cast<T*>(const_cast<char*>(vector_.data_.data()));
    }

    T& operator[](size_t idx) { return *(begin() + idx); }

    const T& operator[](size_t idx) const { return *(cbegin() + idx); }

    T& at(size_t idx) {
      range_check(idx);
      return (*this)[idx];
    }

    const T& at(size_t idx) const {
      range_check(idx);
      return (*this)[idx];
    }

    T& front() { return *begin(); }
    const T& front() const { return *cbegin(); }
    T& back() { return *(end() - 1); }
    const T& back() const { return *(cend() - 1); }

    size_t size() const noexcept { return vector_.size() / sizeof(T); }

    bool empty() const noexcept { return vector_.size() == 0; }

    void reserve(size_t n) { vector_.reserve(n * sizeof(T)); }

    void resize(size_t n) { vector_.resize(n * sizeof(T)); }

    void shrink_to_fit() { vector_.shrink_to_fit(); }

    void clear() { vector_.clear(); }

   private:
    T* AddOneElementToBack() {
      size_t size = vector_.size();
      vector_.data_.resize(size + sizeof(T));
      return reinterpret_cast<T*>(
          const_cast<char*>(vector_.data_.data() + size));
    }

    T* AddElements(const_iterator position, size_t n) {
      ptrdiff_t diff = position - begin();
      vector_.data_.insert(diff * sizeof(T), sizeof(T) * n, 0);
      return reinterpret_cast<T*>(const_cast<char*>(vector_.data_.data())) +
             diff;
    }

    iterator make_iterator(T* ptr) const {
      return iterator(const_cast<T*>(data()), ptr,
                      const_cast<T*>(data() + size()));
    }

    void range_check(size_t idx) const {
      if (idx >= size())
        throw std::out_of_range(base::StringPrintf(
            "StringVector::range_check: (%zd >= %zd)", idx, size()));
    }

    StringVector& vector_;
  };

  StringVector() = default;
  StringVector(const void* s, size_t n)
      : data_(reinterpret_cast<const char*>(s), n) {}
  explicit StringVector(const std::string& data) : data_(data) {}
  explicit StringVector(std::string&& data) noexcept : data_(std::move(data)) {}
  StringVector(const StringVector& other) = default;
  StringVector(StringVector&& other) noexcept : data_(std::move(other.data_)) {}
  ~StringVector() = default;

  StringVector& operator=(const StringVector& other) = default;
  StringVector& operator=(StringVector&& other) = default;

  template <typename T>
  std::enable_if_t<std::is_pointer<T>::value, T> cast() const noexcept {
    return reinterpret_cast<T>(data_.data());
  }

  template <typename T>
  std::enable_if_t<std::is_pointer<T>::value, T> cast() noexcept {
    return reinterpret_cast<T>(const_cast<char*>(data_.data()));
  }

  size_t size() const noexcept { return data_.size(); }

  bool empty() const noexcept { return data_.size() == 0; }

  void reserve(size_t n) { data_.reserve(n); }

  void resize(size_t n) { data_.resize(n); }

  void shrink_to_fit() { data_.shrink_to_fit(); }

  void clear() { data_.clear(); }

  void swap(StringVector& other) { data_.swap(other.data_); }

  const std::string& data() const& noexcept { return data_; }
  std::string&& data() && noexcept { return std::move(data_); }

  template <typename T>
  ConstView<T> AsConstView() const noexcept {
    return ConstView<T>(*this);
  }

  template <typename T>
  View<T> AsView() noexcept {
    return View<T>(*this);
  }

 private:
  template <typename T>
  friend class View;
  template <typename T>
  friend class ConstView;

  std::string data_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_CONTAINERS_STRING_VECTOR_H_