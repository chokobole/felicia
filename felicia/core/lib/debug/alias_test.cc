// Copyright 2018 The Chromium Authors. All rights reserved.
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

#include <string>

#include "absl/memory/memory.h"
#include "gtest/gtest.h"

#include "felicia/core/lib/debug/alias.h"

TEST(DebugAlias, Test) {
  std::unique_ptr<std::string> input =
      absl::make_unique<std::string>("string contents");

  // Verify the contents get copied + the new local variable has the right type.
  DEBUG_ALIAS_FOR_CSTR(copy1, input->c_str(), 100 /* > input->size() */);
  static_assert(sizeof(copy1) == 100,
                "Verification that copy1 has expected size");
  EXPECT_STREQ("string contents", copy1);

  // Verify that the copy is properly null-terminated even when it is smaller
  // than the input string.
  DEBUG_ALIAS_FOR_CSTR(copy2, input->c_str(), 3 /* < input->size() */);
  static_assert(sizeof(copy2) == 3,
                "Verification that copy2 has expected size");
  EXPECT_STREQ("st", copy2);
  EXPECT_EQ('\0', copy2[2]);
}
