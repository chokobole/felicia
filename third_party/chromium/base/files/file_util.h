// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains utility functions for dealing with the local
// filesystem.

#ifndef BASE_FILES_FILE_UTIL_H_
#define BASE_FILES_FILE_UTIL_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <set>
#include <string>
#include <vector>

#if defined(OS_POSIX) || defined(OS_FUCHSIA)
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "base/base_export.h"
#include "base/files/file_path.h"
#include "base/strings/string16.h"
#include "build/build_config.h"

#if defined(OS_WIN)
#include "base/win/windows_types.h"
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
#include "base/logging.h"
#include "base/posix/eintr_wrapper.h"
#endif

namespace base {

// Returns true if the given path exists and is a directory, false otherwise.
BASE_EXPORT bool DirectoryExists(const FilePath& path);

#if defined(OS_POSIX) || defined(OS_FUCHSIA)

// Read exactly |bytes| bytes from file descriptor |fd|, storing the result
// in |buffer|. This function is protected against EINTR and partial reads.
// Returns true iff |bytes| bytes have been successfully read from |fd|.
BASE_EXPORT bool ReadFromFD(int fd, char* buffer, size_t bytes);

#endif  // OS_POSIX || OS_FUCHSIA

// Writes the given buffer into the file, overwriting any data that was
// previously there.  Returns the number of bytes written, or -1 on error.
BASE_EXPORT int WriteFile(const FilePath& filename, const char* data,
                          int size);

#if defined(OS_POSIX) || defined(OS_FUCHSIA)
// Appends |data| to |fd|. Does not close |fd| when done.  Returns true iff
// |size| bytes of |data| were written to |fd|.
BASE_EXPORT bool WriteFileDescriptor(const int fd, const char* data, int size);
#endif

// Sets the given |fd| to non-blocking mode.
// Returns true if it was able to set it in the non-blocking mode, otherwise
// false.
BASE_EXPORT bool SetNonBlocking(int fd);

#if defined(OS_POSIX) || defined(OS_FUCHSIA)
// Creates a non-blocking, close-on-exec pipe.
// This creates a non-blocking pipe that is not intended to be shared with any
// child process. This will be done atomically if the operating system supports
// it. Returns true if it was able to create the pipe, otherwise false.
BASE_EXPORT bool CreateLocalNonBlockingPipe(int fds[2]);

// Sets the given |fd| to close-on-exec mode.
// Returns true if it was able to set it in the close-on-exec mode, otherwise
// false.
BASE_EXPORT bool SetCloseOnExec(int fd);

#endif  // defined(OS_POSIX) || defined(OS_FUCHSIA)

}  // namespace base

#endif  // BASE_FILES_FILE_UTIL_H_
