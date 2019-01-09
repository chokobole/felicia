// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_NET_ERRORS_H_
#define FELICIA_CORE_LIB_NET_ERRORS_H_

#include <string>

#include "felicia/core/lib/base/export.h"

namespace felicia {
namespace net {

// Error values are negative.
enum Error {
  // No error.
  OK = 0,

#define NET_ERROR(label, value) ERR_##label = value,
#include "felicia/core/lib/net/error_list.h"
#undef NET_ERROR
};

// Returns a textual representation of the error code for logging purposes.
EXPORT std::string ErrorToString(int error);

}  // namespace net
}  // namespace felicia

#endif  // FELICIA_CORE_LIB_NET_ERRORS_H_
