#ifndef FELICIA_CORE_COMMUNICATION_SUBSCRIBER_STATE_H_
#define FELICIA_CORE_COMMUNICATION_SUBSCRIBER_STATE_H_

#include "felicia/core/lib/base/state.h"

namespace felicia {
namespace communication {

class SubscriberStateTraits {
 public:
  static constexpr uint8_t kStarted = 0;
  static constexpr uint8_t kStopping = 1;
  static constexpr uint8_t kStopped = 2;

  static constexpr uint8_t InitialState = kStopped;

  static std::string ToString(uint8_t state) {
    switch (state) {
      case kStarted:
        return "Started";
      case kStopping:
        return "Stopping";
      case kStopped:
        return "Stopped";
    }

    NOTREACHED();
    return ::base::EmptyString();
  }
};

class SubscriberState : public State<uint8_t, SubscriberStateTraits> {
 public:
  using State<uint8_t, SubscriberStateTraits>::State;

  ALWAYS_INLINE bool IsStarted() const {
    return state_ == SubscriberStateTraits::kStarted;
  }
  ALWAYS_INLINE bool IsStopped() const {
    return state_ == SubscriberStateTraits::kStopped;
  }
  ALWAYS_INLINE bool IsStopping() const {
    return state_ == SubscriberStateTraits::kStopping;
  }

  ALWAYS_INLINE void ToStarted(const ::base::Location& from_here) {
    set_state(from_here, SubscriberStateTraits::kStarted);
  }
  ALWAYS_INLINE void ToStopping(const ::base::Location& from_here) {
    set_state(from_here, SubscriberStateTraits::kStopping);
  }
  ALWAYS_INLINE void ToStopped(const ::base::Location& from_here) {
    set_state(from_here, SubscriberStateTraits::kStopped);
  }
};

}  // namespace communication
}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_SUBSCRIBER_STATE_H_