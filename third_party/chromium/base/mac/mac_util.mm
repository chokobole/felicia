// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/mac/mac_util.h"

#import <Cocoa/Cocoa.h>

#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_piece.h"

namespace base {
namespace mac {

namespace {

// Returns the running system's Darwin major version. Don't call this, it's
// an implementation detail and its result is meant to be cached by
// MacOSXMinorVersion.
int DarwinMajorVersionInternal() {
  // base::OperatingSystemVersionNumbers calls Gestalt, which is a
  // higher-level operation than is needed. It might perform unnecessary
  // operations. On 10.6, it was observed to be able to spawn threads (see
  // http://crbug.com/53200). It might also read files or perform other
  // blocking operations. Actually, nobody really knows for sure just what
  // Gestalt might do, or what it might be taught to do in the future.
  //
  // uname, on the other hand, is implemented as a simple series of sysctl
  // system calls to obtain the relevant data from the kernel. The data is
  // compiled right into the kernel, so no threads or blocking or other
  // funny business is necessary.

  struct utsname uname_info;
  if (uname(&uname_info) != 0) {
    DPLOG(ERROR) << "uname";
    return 0;
  }

  if (strcmp(uname_info.sysname, "Darwin") != 0) {
    DLOG(ERROR) << "unexpected uname sysname " << uname_info.sysname;
    return 0;
  }

  int darwin_major_version = 0;
  char* dot = strchr(uname_info.release, '.');
  if (dot) {
    if (!base::StringToInt(base::StringPiece(uname_info.release,
                                             dot - uname_info.release),
                           &darwin_major_version)) {
      dot = NULL;
    }
  }

  if (!dot) {
    DLOG(ERROR) << "could not parse uname release " << uname_info.release;
    return 0;
  }

  return darwin_major_version;
}

// Returns the running system's Mac OS X minor version. This is the |y| value
// in 10.y or 10.y.z. Don't call this, it's an implementation detail and the
// result is meant to be cached by MacOSXMinorVersion.
int MacOSXMinorVersionInternal() {
  int darwin_major_version = DarwinMajorVersionInternal();

  // The Darwin major version is always 4 greater than the Mac OS X minor
  // version for Darwin versions beginning with 6, corresponding to Mac OS X
  // 10.2. Since this correspondence may change in the future, warn when
  // encountering a version higher than anything seen before. Older Darwin
  // versions, or versions that can't be determined, result in
  // immediate death.
  CHECK(darwin_major_version >= 6);
  int mac_os_x_minor_version = darwin_major_version - 4;
  DLOG_IF(WARNING, darwin_major_version > 18)
      << "Assuming Darwin " << base::IntToString(darwin_major_version)
      << " is macOS 10." << base::IntToString(mac_os_x_minor_version);

  return mac_os_x_minor_version;
}

}  // namespace

namespace internal {
int MacOSXMinorVersion() {
  static int mac_os_x_minor_version = MacOSXMinorVersionInternal();
  return mac_os_x_minor_version;
}
}  // namespace internal

}  // namespace mac
}  // namespace base
