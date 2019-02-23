// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_util.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "base/files/scoped_file.h"
#include "base/posix/eintr_wrapper.h"
#include "base/threading/scoped_blocking_call.h"
#include "build/build_config.h"

namespace base {

// TODO(chokobole): This is from file.h
#if defined(OS_BSD) || defined(OS_MACOSX) || defined(OS_NACL) || \
    defined(OS_FUCHSIA) || (defined(OS_ANDROID) && __ANDROID_API__ < 21)
typedef struct stat stat_wrapper_t;
#elif defined(OS_POSIX)
typedef struct stat64 stat_wrapper_t;
#endif

namespace {

#if defined(OS_BSD) || defined(OS_MACOSX) || defined(OS_NACL) || \
  defined(OS_FUCHSIA) || (defined(OS_ANDROID) && __ANDROID_API__ < 21)
int CallStat(const char* path, stat_wrapper_t* sb) {
  ScopedBlockingCall scoped_blocking_call(BlockingType::MAY_BLOCK);
  return stat(path, sb);
}
#if !defined(OS_NACL_NONSFI)
int CallLstat(const char* path, stat_wrapper_t* sb) {
  ScopedBlockingCall scoped_blocking_call(BlockingType::MAY_BLOCK);
  return lstat(path, sb);
}
#endif
#else
int CallStat(const char* path, stat_wrapper_t* sb) {
  ScopedBlockingCall scoped_blocking_call(BlockingType::MAY_BLOCK);
  return stat64(path, sb);
}
int CallLstat(const char* path, stat_wrapper_t* sb) {
  ScopedBlockingCall scoped_blocking_call(BlockingType::MAY_BLOCK);
  return lstat64(path, sb);
}
#endif

}  // namespace

bool CreateLocalNonBlockingPipe(int fds[2]) {
#if defined(OS_LINUX)
  return pipe2(fds, O_CLOEXEC | O_NONBLOCK) == 0;
#else
  int raw_fds[2];
  if (pipe(raw_fds) != 0)
    return false;
  ScopedFD fd_out(raw_fds[0]);
  ScopedFD fd_in(raw_fds[1]);
  if (!SetCloseOnExec(fd_out.get()))
    return false;
  if (!SetCloseOnExec(fd_in.get()))
    return false;
  if (!SetNonBlocking(fd_out.get()))
    return false;
  if (!SetNonBlocking(fd_in.get()))
    return false;
  fds[0] = fd_out.release();
  fds[1] = fd_in.release();
  return true;
#endif
}

bool SetNonBlocking(int fd) {
  const int flags = fcntl(fd, F_GETFL);
  if (flags == -1)
    return false;
  if (flags & O_NONBLOCK)
    return true;
  if (HANDLE_EINTR(fcntl(fd, F_SETFL, flags | O_NONBLOCK)) == -1)
    return false;
  return true;
}

bool SetCloseOnExec(int fd) {
#if defined(OS_NACL_NONSFI)
  const int flags = 0;
#else
  const int flags = fcntl(fd, F_GETFD);
  if (flags == -1)
    return false;
  if (flags & FD_CLOEXEC)
    return true;
#endif  // defined(OS_NACL_NONSFI)
  if (HANDLE_EINTR(fcntl(fd, F_SETFD, flags | FD_CLOEXEC)) == -1)
    return false;
  return true;
}

bool DirectoryExists(const FilePath& path) {
  ScopedBlockingCall scoped_blocking_call(BlockingType::MAY_BLOCK);
  stat_wrapper_t file_info;
  if (CallStat(path.value().c_str(), &file_info) != 0)
    return false;
  return S_ISDIR(file_info.st_mode);
}

bool ReadFromFD(int fd, char* buffer, size_t bytes) {
  size_t total_read = 0;
  while (total_read < bytes) {
    ssize_t bytes_read =
        HANDLE_EINTR(read(fd, buffer + total_read, bytes - total_read));
    if (bytes_read <= 0)
      break;
    total_read += bytes_read;
  }
  return total_read == bytes;
}

int WriteFile(const FilePath& filename, const char* data, int size) {
  ScopedBlockingCall scoped_blocking_call(BlockingType::MAY_BLOCK);
  int fd = HANDLE_EINTR(creat(filename.value().c_str(), 0666));
  if (fd < 0)
    return -1;

  int bytes_written = WriteFileDescriptor(fd, data, size) ? size : -1;
  if (IGNORE_EINTR(close(fd)) < 0)
    return -1;
  return bytes_written;
}

bool WriteFileDescriptor(const int fd, const char* data, int size) {
  // Allow for partial writes.
  ssize_t bytes_written_total = 0;
  for (ssize_t bytes_written_partial = 0; bytes_written_total < size;
       bytes_written_total += bytes_written_partial) {
    bytes_written_partial =
        HANDLE_EINTR(write(fd, data + bytes_written_total,
                           size - bytes_written_total));
    if (bytes_written_partial < 0)
      return false;
  }

  return true;
}

}  // namespace base
