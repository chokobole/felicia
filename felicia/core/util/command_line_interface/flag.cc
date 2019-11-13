// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

std::string MakeNamedHelpText(base::StringPiece name, base::StringPiece help,
                              int help_start) {
  int remain_len = help_start;
  std::stringstream ss;
  APPEND_AND_DECREASE_LENGTH(ss, name, remain_len);
  ALIGN_AT_INDEX_AND_APPEND(ss, help, remain_len, help_start);
}

bool ContainsOnlyAsciiAlphaOrDigitOrUndderscore(base::StringPiece text) {
  const char* p = text.data();
  const char* limit = p + text.size();
  while (p < limit) {
    const char c = *p;
    if (!(base::IsAsciiAlpha(c) || base::IsAsciiDigit(c) || c == '_'))
      return false;
    p++;
  }

  return true;
}

bool CheckIfOneOfFlagWasSet(std::vector<std::string>& names) {
  std::string names_txt = base::JoinString(names, ", ");

  std::cerr << kRedError << "Either one of [" << names_txt << "] should be set."
            << std::endl;
  return false;
}

}  // namespace felicia