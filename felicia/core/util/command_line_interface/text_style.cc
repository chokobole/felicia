// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

#define STYLE_METHOD(Style)                              \
  std::string TextStyle::Style(base::StringPiece text) { \
    return ApplyStyle(k##Style, text);                   \
  }

STYLE_METHOD(Red)
STYLE_METHOD(Green)
STYLE_METHOD(Blue)
STYLE_METHOD(Yellow)
STYLE_METHOD(Bold)

#undef STYLE_METHOD

// static
std::string TextStyle::ApplyStyle(const char* style, base::StringPiece text) {
  return base::StrCat({style, text.data(), kNone});
}

}  // namespace felicia