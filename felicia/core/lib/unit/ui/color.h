#ifndef FELICIA_CORE_LIB_UNIT_UI_COLOR_H_
#define FELICIA_CORE_LIB_UNIT_UI_COLOR_H_

#include "third_party/chromium/base/strings/string_number_conversions.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/protobuf/ui.pb.h"

namespace felicia {

template <typename T>
class Color3 {
 public:
  constexpr Color3() : r_(0), g_(0), b_(0) {}
  constexpr Color3(T r, T g, T b) : r_(r), g_(g), b_(b) {}
  constexpr Color3(const Color3& other) = default;
  Color3& operator=(const Color3& other) = default;

  void set_rgb(T r, T g, T b) {
    r_ = r;
    g_ = g;
    b_ = b;
  }
  void set_r(T r) { r_ = r; }
  void set_g(T g) { g_ = g; }
  void set_b(T b) { b_ = b; }

  constexpr T r() const { return r_; }
  constexpr T g() const { return g_; }
  constexpr T b() const { return b_; }

  std::string ToString() const {
    return base::StringPrintf("(%s, %s, %s)", base::NumberToString(r_).c_str(),
                              base::NumberToString(g_).c_str(),
                              base::NumberToString(b_).c_str());
  }

 private:
  T r_;
  T g_;
  T b_;
};

template <typename T>
inline bool operator==(const Color3<T>& lhs, const Color3<T>& rhs) {
  return lhs.r() == rhs.r() && lhs.g() == rhs.g() && lhs.b() == rhs.b();
}

template <typename T>
inline bool operator!=(const Color3<T>& lhs, const Color3<T>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Color3<T>& color) {
  return os << color.ToString();
}

typedef Color3<uint8_t> Color3u;
typedef Color3<float> Color3f;

EXPORT Color3uMessage Color3uToColor3uMessage(const Color3u& color);
EXPORT Color3fMessage Color3fToColor3fMessage(const Color3f& color);

EXPORT Color3u Color3uMessageToColor3u(const Color3uMessage& message);
EXPORT Color3f Color3fMessageToColor3f(const Color3fMessage& message);

template <typename T>
class Color4 {
 public:
  constexpr Color4() : r_(0), g_(0), b_(0), a_(0) {}
  constexpr Color4(T r, T g, T b, T a) : r_(r), g_(g), b_(b), a_(a) {}
  constexpr Color4(const Color4& other) = default;
  Color4& operator=(const Color4& other) = default;

  void set_rgba(T r, T g, T b, T a) {
    r_ = r;
    g_ = g;
    b_ = b;
    a_ = a;
  }
  void set_r(T r) { r_ = r; }
  void set_g(T g) { g_ = g; }
  void set_b(T b) { b_ = b; }
  void set_a(T a) { a_ = a; }

  constexpr T r() const { return r_; }
  constexpr T g() const { return g_; }
  constexpr T b() const { return b_; }
  constexpr T a() const { return a_; }

  std::string ToString() const {
    return base::StringPrintf(
        "(%s, %s, %s, %s)", base::NumberToString(r_).c_str(),
        base::NumberToString(g_).c_str(), base::NumberToString(b_).c_str(),
        base::NumberToString(a_).c_str());
  }

 private:
  T r_;
  T g_;
  T b_;
  T a_;
};

template <typename T>
inline bool operator==(const Color4<T>& lhs, const Color4<T>& rhs) {
  return lhs.r() == rhs.r() && lhs.g() == rhs.g() && lhs.b() == rhs.b() &&
         lhs.a() == rhs.a();
}

template <typename T>
inline bool operator!=(const Color4<T>& lhs, const Color4<T>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Color4<T>& color) {
  return os << color.ToString();
}

typedef Color4<uint8_t> Color4u;
typedef Color4<float> Color4f;

EXPORT Color4uMessage Color4uToColor4uMessage(const Color4u& color);
EXPORT Color4fMessage Color4fToColor4fMessage(const Color4f& color);

EXPORT Color4u Color4uMessageToColor4u(const Color4uMessage& message);
EXPORT Color4f Color4fMessageToColor4f(const Color4fMessage& message);

struct ColorIndexes {
  int r;
  int g;
  int b;
  int a;
};

const ColorIndexes kRGB{0, 1, 2, -1};
const ColorIndexes kRGBA{0, 1, 2, 3};
const ColorIndexes kBGR{2, 1, 0, -1};
const ColorIndexes kBGRA{2, 1, 0, 3};
const ColorIndexes kARGB{1, 2, 3, 0};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_UI_COLOR_H_