#ifndef FELICIA_CORE_COMMUNICATION_REGISTER_STATE_H_
#define FELICIA_CORE_COMMUNICATION_REGISTER_STATE_H_

#include "felicia/core/lib/base/state.h"

// This is the shared class for publisher and subscriber to indicate
// state inside.
namespace felicia {
namespace communication {

class RegisterStateTraits {
 public:
  static constexpr uint8_t kRegistering = 0;
  static constexpr uint8_t kRegistered = 1;
  static constexpr uint8_t kUnregistering = 2;
  static constexpr uint8_t kUnregistered = 3;

  static constexpr uint8_t InitialState = kUnregistered;

  static std::string ToString(uint8_t state) {
    switch (state) {
      case kRegistering:
        return "Registering";
      case kRegistered:
        return "Registered";
      case kUnregistering:
        return "Unregistering";
      case kUnregistered:
        return "Unregistered";
    }

    NOTREACHED();
    return base::EmptyString();
  }
};

class RegisterState : public State<uint8_t, RegisterStateTraits> {
 public:
  using State<uint8_t, RegisterStateTraits>::State;

  ALWAYS_INLINE bool IsRegistering() const {
    return state_ == RegisterStateTraits::kRegistering;
  }
  ALWAYS_INLINE bool IsRegistered() const {
    return state_ == RegisterStateTraits::kRegistered;
  }
  ALWAYS_INLINE bool IsUnregistering() const {
    return state_ == RegisterStateTraits::kUnregistering;
  }
  ALWAYS_INLINE bool IsUnregistered() const {
    return state_ == RegisterStateTraits::kUnregistered;
  }

  ALWAYS_INLINE void ToRegistering(const base::Location& from_here) {
    set_state(from_here, RegisterStateTraits::kRegistering);
  }
  ALWAYS_INLINE void ToRegistered(const base::Location& from_here) {
    set_state(from_here, RegisterStateTraits::kRegistered);
  }
  ALWAYS_INLINE void ToUnregistering(const base::Location& from_here) {
    set_state(from_here, RegisterStateTraits::kUnregistering);
  }
  ALWAYS_INLINE void ToUnregistered(const base::Location& from_here) {
    set_state(from_here, RegisterStateTraits::kUnregistered);
  }
};

}  // namespace communication
}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_REGISTER_STATE_H_