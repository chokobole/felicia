// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MAC_MAC_UTIL_H_
#define BASE_MAC_MAC_UTIL_H_

#include <stdint.h>
#include <string>
#import <AvailabilityMacros.h>
#include <sys/utsname.h>

#include "base/base_export.h"

namespace base {

class FilePath;

namespace mac {

namespace internal {

// Returns the system's Mac OS X minor version. This is the |y| value
// in 10.y or 10.y.z.
BASE_EXPORT int MacOSXMinorVersion();

}  // namespace internal

// Run-time OS version checks. Use these instead of
// base::SysInfo::OperatingSystemVersionNumbers. Prefer the "AtLeast" and
// "AtMost" variants to those that check for a specific version, unless you
// know for sure that you need to check for a specific version.

#define DEFINE_IS_OS_FUNCS(V, TEST_DEPLOYMENT_TARGET) \
  inline bool IsOS10_##V() {                          \
    TEST_DEPLOYMENT_TARGET(>, V, false)               \
    return internal::MacOSXMinorVersion() == V;       \
  }                                                   \
  inline bool IsAtLeastOS10_##V() {                   \
    TEST_DEPLOYMENT_TARGET(>=, V, true)               \
    return internal::MacOSXMinorVersion() >= V;       \
  }                                                   \
  inline bool IsAtMostOS10_##V() {                    \
    TEST_DEPLOYMENT_TARGET(>, V, false)               \
    return internal::MacOSXMinorVersion() <= V;       \
  }

#define TEST_DEPLOYMENT_TARGET(OP, V, RET)                      \
  if (MAC_OS_X_VERSION_MIN_REQUIRED OP MAC_OS_X_VERSION_10_##V) \
    return RET;
#define IGNORE_DEPLOYMENT_TARGET(OP, V, RET)

DEFINE_IS_OS_FUNCS(9, TEST_DEPLOYMENT_TARGET)
DEFINE_IS_OS_FUNCS(10, TEST_DEPLOYMENT_TARGET)

#ifdef MAC_OS_X_VERSION_10_11
DEFINE_IS_OS_FUNCS(11, TEST_DEPLOYMENT_TARGET)
#else
DEFINE_IS_OS_FUNCS(11, IGNORE_DEPLOYMENT_TARGET)
#endif

#ifdef MAC_OS_X_VERSION_10_12
DEFINE_IS_OS_FUNCS(12, TEST_DEPLOYMENT_TARGET)
#else
DEFINE_IS_OS_FUNCS(12, IGNORE_DEPLOYMENT_TARGET)
#endif

#ifdef MAC_OS_X_VERSION_10_13
DEFINE_IS_OS_FUNCS(13, TEST_DEPLOYMENT_TARGET)
#else
DEFINE_IS_OS_FUNCS(13, IGNORE_DEPLOYMENT_TARGET)
#endif

#ifdef MAC_OS_X_VERSION_10_14
DEFINE_IS_OS_FUNCS(14, TEST_DEPLOYMENT_TARGET)
#else
DEFINE_IS_OS_FUNCS(14, IGNORE_DEPLOYMENT_TARGET)
#endif

#undef IGNORE_DEPLOYMENT_TARGET
#undef TEST_DEPLOYMENT_TARGET
#undef DEFINE_IS_OS_FUNCS

// This should be infrequently used. It only makes sense to use this to avoid
// codepaths that are very likely to break on future (unreleased, untested,
// unborn) OS releases, or to log when the OS is newer than any known version.
inline bool IsOSLaterThan10_14_DontCallThis() {
  return !IsAtMostOS10_14();
}

}  // namespace mac
}  // namespace base

#endif  // BASE_MAC_MAC_UTIL_H_
