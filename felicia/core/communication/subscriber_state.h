#ifndef FELICIA_CC_COMMUNICATION_SUBSCRIBER_STATE_H_
#define FELICIA_CC_COMMUNICATION_SUBSCRIBER_STATE_H_

#include <stdint.h>

#include <string>

#include "third_party/chromium/base/compiler_specific.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {
namespace communication {

class SubscriberState {
 private:
  static constexpr uint8_t kStarted = 0;   // for only subscriber
  static constexpr uint8_t kStopping = 1;  // for only subscriber
  static constexpr uint8_t kStopped = 2;   // for only subscriber

 public:
  // Construct with |kStopped|.
  constexpr SubscriberState() {}
  SubscriberState(const SubscriberState& other) : state_(other.state_) {}
  void operator=(const SubscriberState& other) { state_ = other.state_; }

  ALWAYS_INLINE bool IsStarted() const { return state_ == kStarted; }
  ALWAYS_INLINE bool IsStopped() const { return state_ == kStopped; }
  ALWAYS_INLINE bool IsStopping() const { return state_ == kStopping; }

  ALWAYS_INLINE void ToStarted() { set_state(kStarted); }
  ALWAYS_INLINE void ToStopping() { set_state(kStopping); }
  ALWAYS_INLINE void ToStopped() { set_state(kStopped); }

  ALWAYS_INLINE void set_state(uint8_t state) { state_ = state; }

  std::string ToString() const {
    switch (state_) {
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

  ALWAYS_INLINE Status InvalidStateError() const {
    std::string text = ToString();
    return errors::Aborted(
        ::base::StringPrintf("Invalid state(%s)", text.c_str()));
  }

 private:
  uint8_t state_ = kStopped;
};

}  // namespace communication
}  // namespace felicia

#endif  // FELICIA_CC_COMMUNICATION_SUBSCRIBER_STATE_H_