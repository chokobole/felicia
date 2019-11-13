// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/python/type_conversion/util.h"

namespace felicia {

bool IsGilHeld() {
  bool ret = false;
  if (auto tstate = py::detail::get_thread_state_unchecked())
    ret = (tstate == PyGILState_GetThisThreadState());

  return ret;
}

}  // namespace felicia