#ifndef FELICIA_CORE_LIB_UNIT_UI_COLOR_H_
#define FELICIA_CORE_LIB_UNIT_UI_COLOR_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/protobuf/ui.pb.h"

namespace felicia {

class EXPORT Color {
 public:
  Color();
  Color(float r, float y, float b);
  Color(const Color& other);
  Color& operator=(const Color& other);

  void set_rgb(float r, float g, float b);
  void set_r(float r);
  void set_g(float g);
  void set_b(float b);

  float r() const;
  float g() const;
  float b() const;

  ColorMessage ToColorMessage() const;

 private:
  float r_;
  float g_;
  float b_;
};

EXPORT bool operator==(const Color& lhs, const Color& rhs);

EXPORT bool operator!=(const Color& lhs, const Color& rhs);

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