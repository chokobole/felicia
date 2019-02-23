// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_util.h"

#include <windows.h>

#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "base/strings/string_util.h"
#include "base/threading/scoped_blocking_call.h"
#include "base/win/scoped_handle.h"

namespace base {

bool DirectoryExists(const FilePath& path) {
  ScopedBlockingCall scoped_blocking_call(BlockingType::MAY_BLOCK);
  DWORD fileattr = GetFileAttributes(as_wcstr(path.value()));
  if (fileattr != INVALID_FILE_ATTRIBUTES)
    return (fileattr & FILE_ATTRIBUTE_DIRECTORY) != 0;
  return false;
}

int WriteFile(const FilePath& filename, const char* data, int size) {
  ScopedBlockingCall scoped_blocking_call(BlockingType::MAY_BLOCK);
  win::ScopedHandle file(CreateFile(as_wcstr(filename.value()), GENERIC_WRITE,
                                    0, NULL, CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL, NULL));
  if (!file.IsValid()) {
    DPLOG(WARNING) << "CreateFile failed for path "
                   << UTF16ToUTF8(filename.value());
    return -1;
  }

  DWORD written;
  BOOL result = ::WriteFile(file.Get(), data, size, &written, NULL);
  if (result && static_cast<int>(written) == size)
    return written;

  if (!result) {
    // WriteFile failed.
    DPLOG(WARNING) << "writing file " << UTF16ToUTF8(filename.value())
                   << " failed";
  } else {
    // Didn't write all the bytes.
    DLOG(WARNING) << "wrote" << written << " bytes to "
                  << UTF16ToUTF8(filename.value()) << " expected " << size;
  }
  return -1;
}

bool SetNonBlocking(int fd) {
  unsigned long nonblocking = 1;
  if (ioctlsocket(fd, FIONBIO, &nonblocking) == 0)
    return true;
  return false;
}

}  // namespace base
