// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/file/file_util.h"

#include "third_party/chromium/base/files/file.h"

#if defined(OS_POSIX)
#include <fcntl.h>

#include "third_party/chromium/base/posix/eintr_wrapper.h"
#include "third_party/chromium/base/strings/utf_string_conversions.h"
#elif defined(OS_WIN)
#include "third_party/chromium/base/strings/utf_string_conversions.h"
#endif

namespace felicia {

#if defined(OS_POSIX)
bool SetBlocking(int fd, int blocking) {
  int flags = fcntl(fd, F_GETFL);
  if (flags == -1) return false;

  if (blocking) {
    if (!(flags & O_NONBLOCK)) return true;
    flags &= ~O_NONBLOCK;
  } else {
    if (flags & O_NONBLOCK) return true;
    flags |= O_NONBLOCK;
  }

  if (HANDLE_EINTR(fcntl(fd, F_SETFL, flags)) == -1) return false;
  return true;
}
#endif

base::FilePath ToFilePath(const std::string& file_path) {
#if defined(OS_WIN)
  return base::FilePath{base::UTF8ToUTF16(file_path)};
#else
  return base::FilePath{file_path};
#endif
}

bool ReadFile(const base::FilePath& path, std::unique_ptr<char[]>* out,
              size_t* out_len) {
  base::File file(path, base::File::FLAG_OPEN | base::File::FLAG_READ);
  if (!file.IsValid()) return false;
  int64_t length = file.GetLength();
  if (length < 0) return false;
  out->reset(new char[length]);
  *out_len = length;
  int rv = file.ReadAtCurrentPos(out->get(), static_cast<int>(length));
  return rv >= 0;
}

bool WriteFile(const base::FilePath& path, const char* buf, size_t buf_len) {
  base::File file(path,
                  base::File::FLAG_CREATE_ALWAYS | base::File::FLAG_WRITE);
  if (!file.IsValid()) return false;
  int rv = file.WriteAtCurrentPos(buf, buf_len);
  return rv >= 0;
}

}  // namespace felicia
