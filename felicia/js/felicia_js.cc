#include "napi.h"
#include "third_party/chromium/build/build_config.h"

#ifndef OS_WIN
#include "felicia/js/master_proxy_js.h"
#endif
#include "felicia/js/status_js.h"

namespace felicia {

::Napi::Object Init(::Napi::Env env, ::Napi::Object exports) {
  JsStatus::Init(env, exports);
#ifndef OS_WIN
  JsMasterProxy::Init(env, exports);
#endif

  return exports;
}

NODE_API_MODULE(felicia_js, Init)

}  // namespace felicia