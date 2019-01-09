// Copyright 2013 The Chromium Authors. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// =====================================================================
// Modifications copyright (C) 2019 felicia

#include "felicia/core/lib/strings/string_util.h"

namespace felicia {

#define WHITESPACE_UNICODE                    \
  0x0009,     /* CHARACTER TABULATION */      \
      0x000A, /* LINE FEED (LF) */            \
      0x000B, /* LINE TABULATION */           \
      0x000C, /* FORM FEED (FF) */            \
      0x000D, /* CARRIAGE RETURN (CR) */      \
      0x0020, /* SPACE */                     \
      0x0085, /* NEXT LINE (NEL) */           \
      0x00A0, /* NO-BREAK SPACE */            \
      0x1680, /* OGHAM SPACE MARK */          \
      0x2000, /* EN QUAD */                   \
      0x2001, /* EM QUAD */                   \
      0x2002, /* EN SPACE */                  \
      0x2003, /* EM SPACE */                  \
      0x2004, /* THREE-PER-EM SPACE */        \
      0x2005, /* FOUR-PER-EM SPACE */         \
      0x2006, /* SIX-PER-EM SPACE */          \
      0x2007, /* FIGURE SPACE */              \
      0x2008, /* PUNCTUATION SPACE */         \
      0x2009, /* THIN SPACE */                \
      0x200A, /* HAIR SPACE */                \
      0x2028, /* LINE SEPARATOR */            \
      0x2029, /* PARAGRAPH SEPARATOR */       \
      0x202F, /* NARROW NO-BREAK SPACE */     \
      0x205F, /* MEDIUM MATHEMATICAL SPACE */ \
      0x3000, /* IDEOGRAPHIC SPACE */         \
      0

const wchar_t kWhitespaceWide[] = {WHITESPACE_UNICODE};

}  // namespace felicia