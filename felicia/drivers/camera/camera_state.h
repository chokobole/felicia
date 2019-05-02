#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_STATE_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_STATE_H_

#include "third_party/chromium/base/compiler_specific.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

class CameraState {
 private:
  static constexpr uint8_t kStopped = 0;
  static constexpr uint8_t kInitialized = 1;
  static constexpr uint8_t kStarted = 2;

 public:
  // Construct with |kStopped|.
  constexpr CameraState() {}
  CameraState(const CameraState& other) : state_(other.state_) {}
  void operator=(const CameraState& other) { state_ = other.state_; }

  ALWAYS_INLINE bool IsStopped() const { return state_ == kStopped; }
  ALWAYS_INLINE bool IsInitialized() const { return state_ == kInitialized; }
  ALWAYS_INLINE bool IsStarted() const { return state_ == kStarted; }

  ALWAYS_INLINE void ToStopped() { set_state(kStopped); }
  ALWAYS_INLINE void ToInitialized() { set_state(kInitialized); }
  ALWAYS_INLINE void ToStarted() { set_state(kStarted); }

  ALWAYS_INLINE void set_state(uint8_t state) { state_ = state; }

  std::string ToString() const {
    switch (state_) {
      case kStopped:
        return "Stopped";
      case kInitialized:
        return "Initialized";
      case kStarted:
        return "Started";
    }
    NOTREACHED();
    return ::base::EmptyString();
  }

  ALWAYS_INLINE Status InvalidStateError() const {
    std::string text = ToString();
    return errors::Aborted(
        ::base::StringPrintf("Invalid state(%s)", text.c_str()));
  }

 private:
  uint8_t state_ = kStopped;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_STATE_H_