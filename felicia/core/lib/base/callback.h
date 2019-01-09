#ifndef FELICIA_CORE_LIB_BASE_CALLBACK_H_
#define FELICIA_CORE_LIB_BASE_CALLBACK_H_

#include <functional>

namespace felicia {

using CompletionOnceCallback = std::function<void(int)>;
#define CONSUME_COMPLETE_ONCE_CALLBACK(callback_, argument) \
  do {                                                      \
    CompletionOnceCallback callback = callback_;            \
    callback_ = nullptr;                                    \
    callback(argument);                                     \
  } while (false)

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_BASE_CALLBACK_H_
