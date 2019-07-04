#ifndef FELICIA_CORE_LIB_UNIT_UI_COLOR_H_
#define FELICIA_CORE_LIB_UNIT_UI_COLOR_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/protobuf/ui.pb.h"

namespace felicia {

class EXPORT Color3u {
 public:
  Color3u();
  Color3u(uint8_t r, uint8_t y, uint8_t b);
  Color3u(const Color3u& other);
  Color3u& operator=(const Color3u& other);

  void set_rgb(uint8_t r, uint8_t g, uint8_t b);
  void set_r(uint8_t r);
  void set_g(uint8_t g);
  void set_b(uint8_t b);

  uint8_t r() const;
  uint8_t g() const;
  uint8_t b() const;

  Color3uMessage ToColor3uMessage() const;
  Color3fMessage ToColor3fMessage() const;

 private:
  uint8_t r_;
  uint8_t g_;
  uint8_t b_;
};

class EXPORT Color3f {
 public:
  Color3f();
  Color3f(float r, float y, float b);
  Color3f(const Color3f& other);
  Color3f& operator=(const Color3f& other);

  void set_rgb(float r, float g, float b);
  void set_r(float r);
  void set_g(float g);
  void set_b(float b);

  float r() const;
  float g() const;
  float b() const;

  Color3uMessage ToColor3uMessage() const;
  Color3fMessage ToColor3fMessage() const;

 private:
  float r_;
  float g_;
  float b_;
};

EXPORT bool operator==(const Color3u& lhs, const Color3u& rhs);

EXPORT bool operator!=(const Color3u& lhs, const Color3u& rhs);

EXPORT bool operator==(const Color3f& lhs, const Color3f& rhs);

EXPORT bool operator!=(const Color3f& lhs, const Color3f& rhs);

struct EXPORT ColorIndexes {
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