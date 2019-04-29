#include "felicia/core/felicia_init.h"

#include "third_party/chromium/build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/master/master_proxy.h"

namespace felicia {

Status FeliciaInit() {
#if defined(OS_WIN)
  HANDLE a = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  if (!GetConsoleMode(a, &dwMode))
    return errors::FailedPrecondition("Failed to run GetConsoleMode");

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(a, dwMode))
    return errors::FailedPrecondition("Failed to run SetConsoleMode");

  MasterProxy::GetInstance();
#endif
  return Status::OK();
}

}  // namespace felicia