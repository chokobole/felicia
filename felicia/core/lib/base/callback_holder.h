#ifndef FELICIA_CORE_LIB_BASE_CALLBACK_HOLDER_H_
#define FELICIA_CORE_LIB_BASE_CALLBACK_HOLDER_H_

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/macros.h"

namespace felicia {

// TODO(chokobole): Remove this once c++ support lambda move capture.
template <typename Signature>
struct OnceCallbackHolder;

template <typename R, typename... Args>
class OnceCallbackHolder<R(Args...)> {
 public:
  explicit OnceCallbackHolder(base::OnceCallback<R(Args...)> callback)
      : callback_(std::move(callback)) {}

  R Invoke(Args... args) {
    return std::move(callback_).Run(std::forward<Args>(args)...);
  }

 private:
  base::OnceCallback<R(Args...)> callback_;
};

template <typename Signature>
struct ScopedOnceCallbackHolder;

template <typename R, typename... Args>
struct ScopedOnceCallbackHolder<R(Args...)> {
  explicit ScopedOnceCallbackHolder(
      OnceCallbackHolder<R(Args...)>* callback_holder)
      : callback_holder(callback_holder) {}
  ~ScopedOnceCallbackHolder() { delete callback_holder; }

  OnceCallbackHolder<R(Args...)>* callback_holder;

  DISALLOW_COPY_AND_ASSIGN(ScopedOnceCallbackHolder);
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_BASE_CALLBACK_HOLDER_H_