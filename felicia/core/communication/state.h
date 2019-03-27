#ifndef FELICIA_CC_COMMUNICATION_STATE_H_
#define FELICIA_CC_COMMUNICATION_STATE_H_

#include "third_party/chromium/base/compiler_specific.h"

#include <stdint.h>

#include <string>

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

// This is the shared class for publisher and subscriber to indicate
// state inside.
namespace felicia {
namespace communication {

class State {
 private:
  static constexpr uint8_t kRegistering = 0;
  static constexpr uint8_t kRegistered = 1;
  static constexpr uint8_t kUnregistering = 2;
  static constexpr uint8_t kUnregistered = 3;
  static constexpr uint8_t kStarted = 4;  // for only subscriber
  static constexpr uint8_t kStopped = 5;  // for only subscriber
  static constexpr uint8_t kUnknown = 6;  // for only subscriber

 public:
  // Construct with |kUnknown|.
  constexpr State() {}
  State(const State& other) : state_(other.state_) {}
  void operator=(const State& other) { state_ = other.state_; }

  ALWAYS_INLINE bool IsRegistering() const { return state_ == kRegistering; }
  ALWAYS_INLINE bool IsRegistered() const { return state_ == kRegistered; }
  ALWAYS_INLINE bool IsUnregistering() const {
    return state_ == kUnregistering;
  }
  ALWAYS_INLINE bool IsUnregistered() const { return state_ == kUnregistered; }
  ALWAYS_INLINE bool IsStarted() const { return state_ == kStarted; }
  ALWAYS_INLINE bool IsStopped() const { return state_ == kStopped; }
  ALWAYS_INLINE bool IsUnknown() const { return state_ == kUnknown; }

  ALWAYS_INLINE void ToRegistering() { set_state(kRegistering); }
  ALWAYS_INLINE void ToRegistered() { set_state(kRegistered); }
  ALWAYS_INLINE void ToUnregistering() { set_state(kUnregistering); }
  ALWAYS_INLINE void ToUneregistered() { set_state(kUnregistered); }
  ALWAYS_INLINE void ToStarted() { set_state(kStarted); }
  ALWAYS_INLINE void ToStopped() { set_state(kStopped); }
  ALWAYS_INLINE void ToUnknown() { set_state(kUnknown); }

  ALWAYS_INLINE void set_state(uint8_t state) { state_ = state; }

  std::string ToString() const {
    switch (state_) {
      case kRegistering:
        return "Registering";
      case kRegistered:
        return "Registered";
      case kUnregistering:
        return "Unregistering";
      case kUnregistered:
        return "Unregistered";
      case kStarted:
        return "Started";
      case kStopped:
        return "Stopped";
    }

    return "Unknown";
  }

  ALWAYS_INLINE Status InvalidStateError() const {
    std::string text = ToString();
    return errors::Aborted(
        ::base::StringPrintf("Invalid state(%s)", text.c_str()));
  }

 private:
  uint8_t state_ = kUnknown;
};

}  // namespace communication
}  // namespace felicia

#endif  // FELICIA_CC_COMMUNICATION_STATE_H_