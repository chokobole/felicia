// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/strings/str_util.h"

#include <algorithm>

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/utf_string_conversions.h"

#include "felicia/core/lib/strings/str_util.h"

namespace felicia {

bool ConsumePrefix(base::StringPiece* s, base::StringPiece expected) {
  if (StartsWith(*s, expected)) {
    s->remove_prefix(expected.size());
    return true;
  }
  return false;
}

bool ConsumeSuffix(base::StringPiece* s, base::StringPiece expected) {
  if (EndsWith(*s, expected)) {
    s->remove_suffix(expected.size());
    return true;
  }
  return false;
}

bool StartsWith(base::StringPiece s, base::StringPiece expected) {
  return base::StartsWith(s, expected, base::CompareCase::SENSITIVE);
}

bool EndsWith(base::StringPiece s, base::StringPiece expected) {
  return base::EndsWith(s, expected, base::CompareCase::SENSITIVE);
}

bool Contains(base::StringPiece s, base::StringPiece expected) {
  return s.find(expected) != base::StringPiece::npos;
}

std::string BoolToString(bool b) { return b ? "true" : "false"; }

}  // namespace felicia
