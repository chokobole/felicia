// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/felicia_init.h"

#include "third_party/chromium/build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

#include "felicia/core/lib/error/errors.h"

namespace felicia {

Status FeliciaInit() {
#if defined(OS_WIN)
  HANDLE a = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  if (!GetConsoleMode(a, &dwMode))
    return errors::FailedPrecondition("Failed to run GetConsoleMode");

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(a, dwMode))
    return errors::FailedPrecondition("Failed to run SetConsoleMode");
#endif
  return Status::OK();
}

}  // namespace felicia