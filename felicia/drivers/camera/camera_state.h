#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_STATE_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_STATE_H_

#include "felicia/core/lib/base/state.h"

namespace felicia {

class CameraStateTraits {
 public:
  static constexpr uint8_t kStopped = 0;
  static constexpr uint8_t kInitialized = 1;
  static constexpr uint8_t kStarted = 2;

  static constexpr uint8_t InitialState = kStopped;

  static std::string ToString(uint8_t state) {
    switch (state) {
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
};

class CameraState : public State<uint8_t, CameraStateTraits> {
 public:
  using State<uint8_t, CameraStateTraits>::State;

  ALWAYS_INLINE bool IsStopped() const {
    return state_ == CameraStateTraits::kStopped;
  }
  ALWAYS_INLINE bool IsInitialized() const {
    return state_ == CameraStateTraits::kInitialized;
  }
  ALWAYS_INLINE bool IsStarted() const {
    return state_ == CameraStateTraits::kStarted;
  }

  ALWAYS_INLINE void ToStopped() { set_state(CameraStateTraits::kStopped); }
  ALWAYS_INLINE void ToInitialized() {
    set_state(CameraStateTraits::kInitialized);
  }
  ALWAYS_INLINE void ToStarted() { set_state(CameraStateTraits::kStarted); }
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_STATE_H_