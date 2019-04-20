#include "napi.h"

#include "felicia/js/master_proxy_js.h"
#include "felicia/js/status_js.h"

namespace felicia {

::Napi::Object Init(::Napi::Env env, ::Napi::Object exports) {
  JsStatus::Init(env, exports);
  JsMasterProxy::Init(env, exports);

  return exports;
}

NODE_API_MODULE(felicia_js, Init)

}  // namespace felicia