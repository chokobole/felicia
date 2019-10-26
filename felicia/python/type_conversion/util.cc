#include "felicia/python/type_conversion/util.h"

namespace felicia {

bool IsGilHeld() {
  bool ret = false;
  if (auto tstate = py::detail::get_thread_state_unchecked())
    ret = (tstate == PyGILState_GetThisThreadState());

  return ret;
}

}  // namespace felicia