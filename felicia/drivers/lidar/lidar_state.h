#ifndef FELICIA_DRIVERS_LIDAR_LIDAR_STATE_H_
#define FELICIA_DRIVERS_LIDAR_LIDAR_STATE_H_

#include "felicia/core/lib/base/state.h"

namespace felicia {
namespace drivers {

class LidarStateTraits {
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
    return base::EmptyString();
  }
};

class LidarState : public State<uint8_t, LidarStateTraits> {
 public:
  using State<uint8_t, LidarStateTraits>::State;

  ALWAYS_INLINE bool IsStopped() const {
    return state_ == LidarStateTraits::kStopped;
  }
  ALWAYS_INLINE bool IsInitialized() const {
    return state_ == LidarStateTraits::kInitialized;
  }
  ALWAYS_INLINE bool IsStarted() const {
    return state_ == LidarStateTraits::kStarted;
  }

  ALWAYS_INLINE void ToStopped() { set_state(LidarStateTraits::kStopped); }
  ALWAYS_INLINE void ToInitialized() {
    set_state(LidarStateTraits::kInitialized);
  }
  ALWAYS_INLINE void ToStarted() { set_state(LidarStateTraits::kStarted); }
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_LIDAR_LIDAR_STATE_H_