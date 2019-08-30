#ifndef FELICIA_CORE_LIB_CONTAINERS_DATA_H_
#define FELICIA_CORE_LIB_CONTAINERS_DATA_H_

#include <string>
#include <type_traits>

#include "third_party/chromium/base/containers/checked_iterators.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/containers/data_constants.h"
#include "felicia/core/lib/containers/data_internal.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/core/lib/unit/ui/color.h"
#include "felicia/core/protobuf/data.pb.h"

namespace felicia {

template <typename T>
struct DataMessageTraits;

template <>
struct DataMessageTraits<void> {
  static constexpr DataMessage::ElementType element_type =
      DataMessage::ELEMENT_TYPE_CUSTOM;
  static constexpr uint32_t data_type = DATA_TYPE_CUSTOM_C1;
};

template <>
struct DataMessageTraits<uint8_t> {
  static constexpr DataMessage::ElementType element_type =
      DataMessage::ELEMENT_TYPE_8U;
  static constexpr uint32_t data_type = DATA_TYPE_8U_C1;
};

template <>
struct DataMessageTraits<int8_t> {
  static constexpr DataMessage::ElementType element_type =
      DataMessage::ELEMENT_TYPE_8S;
  static constexpr uint32_t data_type = DATA_TYPE_8S_C1;
};

template <>
struct DataMessageTraits<uint16_t> {
  static constexpr DataMessage::ElementType element_type =
      DataMessage::ELEMENT_TYPE_16U;
  static constexpr uint32_t data_type = DATA_TYPE_16U_C1;
};

template <>
struct DataMessageTraits<int16_t> {
  static constexpr DataMessage::ElementType element_type =
      DataMessage::ELEMENT_TYPE_16S;
  static constexpr uint32_t data_type = DATA_TYPE_16S_C1;
};

template <>
struct DataMessageTraits<uint32_t> {
  static constexpr DataMessage::ElementType element_type =
      DataMessage::ELEMENT_TYPE_32U;
  static constexpr uint32_t data_type = DATA_TYPE_32U_C1;
};

template <>
struct DataMessageTraits<int32_t> {
  static constexpr DataMessage::ElementType element_type =
      DataMessage::ELEMENT_TYPE_32S;
  static constexpr uint32_t data_type = DATA_TYPE_32S_C1;
};

template <>
struct DataMessageTraits<uint64_t> {
  static constexpr DataMessage::ElementType element_type =
      DataMessage::ELEMENT_TYPE_64U;
  static constexpr uint32_t data_type = DATA_TYPE_64U_C1;
};

template <>
struct DataMessageTraits<int64_t> {
  static constexpr DataMessage::ElementType element_type =
      DataMessage::ELEMENT_TYPE_64S;
  static constexpr uint32_t data_type = DATA_TYPE_64S_C1;
};

template <>
struct DataMessageTraits<float> {
  static constexpr DataMessage::ElementType element_type =
      DataMessage::ELEMENT_TYPE_32F;
  static constexpr uint32_t data_type = DATA_TYPE_32F_C1;
};

template <>
struct DataMessageTraits<double> {
  static constexpr DataMessage::ElementType element_type =
      DataMessage::ELEMENT_TYPE_64F;
  static constexpr uint32_t data_type = DATA_TYPE_64F_C1;
};

template <typename T>
struct DataMessageTraits<Point<T>> {
  static constexpr DataMessage::ElementType element_type =
      DataMessageTraits<T>::element_type;
  static constexpr uint32_t data_type =
      internal::MakeDataMessageType(element_type, DataMessage::CHANNEL_TYPE_C2);
};

template <typename T>
struct DataMessageTraits<Point3<T>> {
  static constexpr DataMessage::ElementType element_type =
      DataMessageTraits<T>::element_type;
  static constexpr uint32_t data_type =
      internal::MakeDataMessageType(element_type, DataMessage::CHANNEL_TYPE_C3);
};

template <typename T>
struct DataMessageTraits<Color3<T>> {
  static constexpr DataMessage::ElementType element_type =
      DataMessageTraits<T>::element_type;
  static constexpr uint32_t data_type =
      internal::MakeDataMessageType(element_type, DataMessage::CHANNEL_TYPE_C3);
};

template <typename T>
struct DataMessageTraits<Color4<T>> {
  static constexpr DataMessage::ElementType element_type =
      DataMessageTraits<T>::element_type;
  static constexpr uint32_t data_type =
      internal::MakeDataMessageType(element_type, DataMessage::CHANNEL_TYPE_C4);
};

// Data is a container which has std::string inside, but is able to
// be used like std::vector. It is used as a c++ implementation against
// DataMessage type of protobuf. If you want to use protobuf bytes type, but
// in c++ it can be float or int, you can use this class like below.
// (e.g, drivers::CameraFrame has one container which can be uint8_t array,
// uint16_t array, or anything.)
//
// Data data;
// Data::View<int> view = data.AsView<int>();
// or
// Data::View<float> view = data.AsView<float>();
class EXPORT Data {
 public:
  template <typename T>
  class ConstView {
   public:
    static_assert(std::is_trivially_destructible<T>::value,
                  "Data type needs to be trivially_destructible.");

    using const_iterator = base::CheckedRandomAccessConstIterator<T>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    explicit ConstView(const Data& data) : data_(data) {}

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
      return reinterpret_cast<const T*>(data_.data_.data());
    }

    const T& operator[](size_t idx) const { return *(cbegin() + idx); }

    const T& at(size_t idx) const {
      range_check(idx);
      return (*this)[idx];
    }

    const T& front() const { return *cbegin(); }
    const T& back() const { return *(cend() - 1); }

    size_t size() const noexcept { return data_.size() / sizeof(T); }

    bool empty() const noexcept { return data_.size() == 0; }

   private:
    void range_check(size_t idx) const {
      if (idx >= size())
        throw std::out_of_range(
            base::StringPrintf("Data::range_check: (%zd >= %zd)", idx, size()));
    }

    const_iterator make_iterator(T* ptr) const {
      return const_iterator(const_cast<T*>(data()), ptr,
                            const_cast<T*>(data() + size()));
    }

    const Data& data_;
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

    explicit View(Data& data) : data_(data) {}

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

    void pop_back() { data_.resize(data_.size() - sizeof(T)); }

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
      data_.data_.erase(diff * sizeof(T), sizeof(T));
      return begin() + diff;
    }

    iterator erase(const_iterator first, const_iterator last) {
      ptrdiff_t diff = first - begin();
      data_.data_.erase(diff * sizeof(T), (last - first) * sizeof(T));
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
      return reinterpret_cast<const T*>(data_.data_.data());
    }
    T* data() {
      return reinterpret_cast<T*>(const_cast<char*>(data_.data_.data()));
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

    size_t size() const noexcept { return data_.size() / sizeof(T); }

    bool empty() const noexcept { return data_.size() == 0; }

    void reserve(size_t n) { data_.reserve(n * sizeof(T)); }

    void resize(size_t n) { data_.resize(n * sizeof(T)); }

    void shrink_to_fit() { data_.shrink_to_fit(); }

    void clear() { data_.clear(); }

   private:
    T* AddOneElementToBack() {
      size_t size = data_.size();
      data_.data_.resize(size + sizeof(T));
      return reinterpret_cast<T*>(const_cast<char*>(data_.data_.data() + size));
    }

    T* AddElements(const_iterator position, size_t n) {
      ptrdiff_t diff = position - begin();
      data_.data_.insert(diff * sizeof(T), sizeof(T) * n, 0);
      return reinterpret_cast<T*>(const_cast<char*>(data_.data_.data())) + diff;
    }

    iterator make_iterator(T* ptr) const {
      return iterator(const_cast<T*>(data()), ptr,
                      const_cast<T*>(data() + size()));
    }

    void range_check(size_t idx) const {
      if (idx >= size())
        throw std::out_of_range(
            base::StringPrintf("Data::range_check: (%zd >= %zd)", idx, size()));
    }

    Data& data_;
  };

  Data();
  template <typename T>
  Data(const T* s, size_t n, uint32_t type = DataMessageTraits<T>::data_type)
      : data_(reinterpret_cast<const char*>(s), n), type_(type) {}
  Data(const std::string& data, uint32_t type = DATA_TYPE_CUSTOM_C1);
  Data(std::string&& data, uint32_t type = DATA_TYPE_CUSTOM_C1) noexcept;
  Data(const Data& other);
  Data(Data&& other) noexcept;
  ~Data();

  Data& operator=(const Data& other);
  Data& operator=(Data&& other);

  template <typename T>
  std::enable_if_t<std::is_pointer<T>::value, T> cast() const noexcept {
    return reinterpret_cast<T>(data_.data());
  }

  template <typename T>
  std::enable_if_t<std::is_pointer<T>::value, T> cast() noexcept {
    return reinterpret_cast<T>(const_cast<char*>(data_.data()));
  }

  size_t size() const noexcept;

  bool empty() const noexcept;

  void reserve(size_t n);

  void resize(size_t n);

  void shrink_to_fit();

  void clear();

  void swap(Data& other);

  const std::string& data() const& noexcept;
  std::string&& data() && noexcept;

  uint32_t type() const;
  void set_type(uint32_t type);

  void GetElementaAndChannelType(DataMessage::ElementType* element_type,
                                 DataMessage::ChannelType* channel_type);

  template <typename T>
  ConstView<T> AsConstView() const noexcept {
    return ConstView<T>(*this);
  }

  template <typename T>
  View<T> AsView() noexcept {
    return View<T>(*this);
  }

  DataMessage ToDataMessage(bool copy = true);
  Status FromDataMessage(const DataMessage& message);
  Status FromDataMessage(DataMessage&& message);

 private:
  template <typename T>
  friend class View;
  template <typename T>
  friend class ConstView;

  std::string data_;
  uint32_t type_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_CONTAINERS_DATA_H_