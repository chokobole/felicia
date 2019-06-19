#include "felicia/core/lib/unit/ui/color.h"

namespace felicia {

Color::Color() = default;

Color::Color(float r, float g, float b) : r_(r), g_(g), b_(b) {}

Color::Color(const Color& other) = default;

Color& Color::operator=(const Color& other) = default;

void Color::set_rgb(float r, float g, float b) {
  r_ = r;
  g_ = g;
  b_ = b;
}

void Color::set_r(float r) { r_ = r; }

void Color::set_g(float g) { g_ = g; }

void Color::set_b(float b) { b_ = b; }

float Color::r() const { return r_; }

float Color::g() const { return g_; }

float Color::b() const { return b_; }

ColorMessage Color::ToColorMessage() const {
  ColorMessage message;
  message.set_r(r_);
  message.set_g(g_);
  message.set_b(b_);
  return message;
}

bool operator==(const Color& lhs, const Color& rhs) {
  return lhs.r() == rhs.r() && lhs.g() == rhs.g() && lhs.b() == rhs.b();
}

bool operator!=(const Color& lhs, const Color& rhs) { return !(lhs == rhs); }

}  // namespace felicia