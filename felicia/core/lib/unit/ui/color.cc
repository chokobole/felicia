#include "felicia/core/lib/unit/ui/color.h"

namespace felicia {

Color3u::Color3u() = default;

Color3u::Color3u(uint8_t r, uint8_t g, uint8_t b) : r_(r), g_(g), b_(b) {}

Color3u::Color3u(const Color3u& other) = default;

Color3u& Color3u::operator=(const Color3u& other) = default;

void Color3u::set_rgb(uint8_t r, uint8_t g, uint8_t b) {
  r_ = r;
  g_ = g;
  b_ = b;
}

void Color3u::set_r(uint8_t r) { r_ = r; }

void Color3u::set_g(uint8_t g) { g_ = g; }

void Color3u::set_b(uint8_t b) { b_ = b; }

uint8_t Color3u::r() const { return r_; }

uint8_t Color3u::g() const { return g_; }

uint8_t Color3u::b() const { return b_; }

Color3uMessage Color3u::ToColor3uMessage() const {
  Color3uMessage message;
  message.set_r(r_);
  message.set_g(g_);
  message.set_b(b_);
  return message;
}

Color3fMessage Color3u::ToColor3fMessage() const {
  Color3fMessage message;
  message.set_r(r_ / 255.f);
  message.set_g(g_ / 255.f);
  message.set_b(b_ / 255.f);
  return message;
}

Color3f::Color3f() = default;

Color3f::Color3f(float r, float g, float b) : r_(r), g_(g), b_(b) {}

Color3f::Color3f(const Color3f& other) = default;

Color3f& Color3f::operator=(const Color3f& other) = default;

void Color3f::set_rgb(float r, float g, float b) {
  r_ = r;
  g_ = g;
  b_ = b;
}

void Color3f::set_r(float r) { r_ = r; }

void Color3f::set_g(float g) { g_ = g; }

void Color3f::set_b(float b) { b_ = b; }

float Color3f::r() const { return r_; }

float Color3f::g() const { return g_; }

float Color3f::b() const { return b_; }

Color3uMessage Color3f::ToColor3uMessage() const {
  Color3uMessage message;
  message.set_r(r_ * 255);
  message.set_g(g_ * 255);
  message.set_b(b_ * 255);
  return message;
}

Color3fMessage Color3f::ToColor3fMessage() const {
  Color3fMessage message;
  message.set_r(r_);
  message.set_g(g_);
  message.set_b(b_);
  return message;
}

bool operator==(const Color3u& lhs, const Color3u& rhs) {
  return lhs.r() == rhs.r() && lhs.g() == rhs.g() && lhs.b() == rhs.b();
}

bool operator!=(const Color3u& lhs, const Color3u& rhs) {
  return !(lhs == rhs);
}

bool operator==(const Color3f& lhs, const Color3f& rhs) {
  return lhs.r() == rhs.r() && lhs.g() == rhs.g() && lhs.b() == rhs.b();
}

bool operator!=(const Color3f& lhs, const Color3f& rhs) {
  return !(lhs == rhs);
}

}  // namespace felicia