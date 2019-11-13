// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_BASE_STATE_H_
#define FELICIA_CORE_LIB_BASE_STATE_H_

#include <stdint.h>

#include <string>

#include "third_party/chromium/base/compiler_specific.h"
#include "third_party/chromium/base/location.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

template <typename T, typename Traits>
class State {
 public:
  State() : state_(Traits::InitialState) {}
  State(const State& other) : state_(other.state_) {}
  void operator=(const State& other) { state_ = other.state_; }

  ALWAYS_INLINE void set_state(const base::Location& from_here, T state) {
    DLOG(INFO) << from_here.ToString() << ": " << Traits::ToString(state_)
               << " -> " << Traits::ToString(state);
    state_ = state;
  }

  ALWAYS_INLINE void set_state(T state) { state_ = state; }

  std::string ToString() const { return Traits::ToString(state_); }

  ALWAYS_INLINE Status InvalidStateError() const {
    std::string text = ToString();
    return errors::Aborted(
        base::StringPrintf("Invalid state(%s)", text.c_str()));
  }

 protected:
  T state_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_BASE_STATE_H_