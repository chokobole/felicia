// Copyright 2014 The Chromium Authors. All rights reserved.
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

#ifndef FELICIA_CORE_LIB_FILES_SCOPED_FILE_H_
#define FELICIA_CORE_LIB_FILES_SCOPED_FILE_H_

#include <stdio.h>

#include <memory>

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/logging.h"
#include "felicia/core/lib/base/platform.h"
#include "felicia/core/lib/base/scoped_generic.h"

namespace felicia {

namespace internal {

#if defined(PLATFORM_ANDROID)
// Use fdsan on android.
struct EXPORT ScopedFDCloseTraits : public ScopedGenericOwnershipTracking {
  static int InvalidValue() { return -1; }
  static void Free(int);
  static void Acquire(const ScopedGeneric<int, ScopedFDCloseTraits>&, int);
  static void Release(const ScopedGeneric<int, ScopedFDCloseTraits>&, int);
};
#elif defined(PLATFORM_POSIX)
struct EXPORT ScopedFDCloseTraits {
  static int InvalidValue() { return -1; }
  static void Free(int fd);
};
#endif

// Functor for |ScopedFILE| (below).
struct ScopedFILECloser {
  inline void operator()(FILE* x) const {
    if (x) fclose(x);
  }
};

}  // namespace internal

// -----------------------------------------------------------------------------

#if defined(PLATFORM_POSIX)
// A low-level Posix file descriptor closer class. Use this when writing
// platform-specific code, especially that does non-file-like things with the
// FD (like sockets).
//
// If you're writing low-level Windows code, see felicia/win/scoped_handle.h
// which provides some additional functionality.
//
// If you're writing cross-platform code that deals with actual files, you
// should generally use felicia::File instead which can be constructed with a
// handle, and in addition to handling ownership, has convenient cross-platform
// file manipulation functions on it.
typedef ScopedGeneric<int, internal::ScopedFDCloseTraits> ScopedFD;
#endif

// Automatically closes |FILE*|s.
typedef std::unique_ptr<FILE, internal::ScopedFILECloser> ScopedFILE;

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_FILES_SCOPED_FILE_H_
