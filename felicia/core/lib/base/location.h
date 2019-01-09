// Copyright (c) 2012 The Chromium Authors. All rights reserved.
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

#ifndef FELICIA_CORE_LIB_BASE_LOCATION_H_
#define FELICIA_CORE_LIB_BASE_LOCATION_H_

#include <stddef.h>

#include <cassert>
#include <string>

#include "absl/hash/hash.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

// Location provides basic info where of an object was constructed, or was
// significantly brought to life.
class EXPORT Location {
 public:
  Location();
  Location(const Location& other);

  // Only initializes the file name and program counter, the source information
  // will be null for the strings, and -1 for the line number.
  // TODO(http://crbug.com/760702) remove file name from this constructor.
  Location(const char* file_name, const void* program_counter);

  // Constructor should be called with a long-lived char*, such as __FILE__.
  // It assumes the provided value will persist as a global constant, and it
  // will not make a copy of it.
  Location(const char* function_name, const char* file_name, int line_number,
           const void* program_counter);

  // Comparator for hash map insertion. The program counter should uniquely
  // identify a location.
  bool operator==(const Location& other) const {
    return program_counter_ == other.program_counter_;
  }

  // Returns true if there is source code location info. If this is false,
  // the Location object only contains a program counter or is
  // default-initialized (the program counter is also null).
  bool has_source_info() const { return function_name_ && file_name_; }

  // Will be nullptr for default initialized Location objects and when source
  // names are disabled.
  const char* function_name() const { return function_name_; }

  // Will be nullptr for default initialized Location objects and when source
  // names are disabled.
  const char* file_name() const { return file_name_; }

  // Will be -1 for default initialized Location objects and when source names
  // are disabled.
  int line_number() const { return line_number_; }

  // The address of the code generating this Location object. Should always be
  // valid except for default initialized Location objects, which will be
  // nullptr.
  const void* program_counter() const { return program_counter_; }

  // Converts to the most user-readable form possible. If function and filename
  // are not available, this will return "pc:<hex address>".
  std::string ToString() const;

  static Location CreateFromHere(const char* file_name);
  static Location CreateFromHere(const char* function_name,
                                 const char* file_name, int line_number);

  template <typename H>
  friend H AbslHashValue(H h, const Location& loc) {
    const void* program_counter = loc.program_counter();
    return H::combine_contiguous(std::move(h), &program_counter, sizeof(void*));
  }

 private:
  const char* function_name_ = nullptr;
  const char* file_name_ = nullptr;
  int line_number_ = -1;
  const void* program_counter_ = nullptr;
};

EXPORT const void* GetProgramCounter();

// Full source information should be included.
#define FROM_HERE FROM_HERE_WITH_EXPLICIT_FUNCTION(__func__)
#define FROM_HERE_WITH_EXPLICIT_FUNCTION(function_name) \
  ::felicia::Location::CreateFromHere(function_name, __FILE__, __LINE__)

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_BASE_LOCATION_H_
