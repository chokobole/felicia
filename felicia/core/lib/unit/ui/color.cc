// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/unit/ui/color.h"

#include "third_party/chromium/base/bit_cast.h"

namespace felicia {

namespace {

struct Color8_4 {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;

  Color8_4() = default;
  explicit Color8_4(const Color3u& color)
      : r(color.r()), g(color.g()), b(color.b()), a(255) {}
  explicit Color8_4(const Color4u& color)
      : r(color.r()), g(color.g()), b(color.b()), a(color.a()) {}
};

uint32_t Color8_4ToUint32(const Color8_4& color) {
  return bit_cast<uint32_t>(color);
}

Color8_4 Uint32ToColor8_4(uint32_t color) { return bit_cast<Color8_4>(color); }

}  // namespace

Color3uMessage Color3uToColor3uMessage(const Color3u& color) {
  Color3uMessage message;
  message.set_rgb(Color8_4ToUint32(Color8_4(color)));
  return message;
}

Color3fMessage Color3fToColor3fMessage(const Color3f& color) {
  Color3fMessage message;
  message.set_r(color.r());
  message.set_g(color.g());
  message.set_b(color.b());
  return message;
}

Color3u Color3uMessageToColor3u(const Color3uMessage& message) {
  Color8_4 color = Uint32ToColor8_4(message.rgb());
  return {color.r, color.g, color.b};
}

Color3f Color3fMessageToColor3f(const Color3fMessage& message) {
  return {message.r(), message.g(), message.b()};
}

Color4uMessage Color4uToColor4uMessage(const Color4u& color) {
  Color4uMessage message;
  message.set_rgba(Color8_4ToUint32(Color8_4(color)));
  return message;
}

Color4fMessage Color4fToColor4fMessage(const Color4f& color) {
  Color4fMessage message;
  message.set_r(color.r());
  message.set_g(color.g());
  message.set_b(color.b());
  message.set_a(color.a());
  return message;
}

Color4u Color4uMessageToColor4u(const Color4uMessage& message) {
  Color8_4 color = Uint32ToColor8_4(message.rgba());
  return {color.r, color.g, color.b, color.a};
}

Color4f Color4fMessageToColor4f(const Color4fMessage& message) {
  return {message.r(), message.g(), message.b(), message.a()};
}

}  // namespace felicia