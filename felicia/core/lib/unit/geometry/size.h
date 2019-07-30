#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_SIZE_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_SIZE_H_

#include "third_party/chromium/base/numerics/safe_math.h"

namespace felicia {

template <typename T>
class Size {
 public:
  constexpr Size() = default;
  constexpr Size(T width, T height)
      : width_(width > 0 ? width : 0), height_(height > 0 ? height : 0) {}
  constexpr Size(const Size& other) = default;
  Size& operator=(const Size& other) = default;

  constexpr T width() const { return width_; }
  constexpr T height() const { return height_; }

  void set_width(T width) { width_ = width; }
  void set_height(T height) { height_ = height; }

  constexpr size_t area() const { return CheckedArea().ValueOrDie(); }

 private:
  constexpr ::base::CheckedNumeric<size_t> CheckedArea() const {
    ::base::CheckedNumeric<size_t> checked_area = width();
    checked_area *= height();
    return checked_area;
  }

  T width_ = 0;
  T height_ = 0;
};

template <typename T>
inline bool operator==(const Size<T>& lhs, const Size<T>& rhs) {
  return lhs.width() == rhs.width() && lhs.height() == rhs.height();
}

template <typename T>
inline bool operator!=(const Size<T>& lhs, const Size<T>& rhs) {
  return !(lhs == rhs);
}

typedef Size<int> Sizei;
typedef Size<float> Sizef;
typedef Size<double> Sized;

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_SIZE_H_