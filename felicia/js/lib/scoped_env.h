#ifndef FELICIA_JS_LIB_SCOPED_ENV_H_
#define FELICIA_JS_LIB_SCOPED_ENV_H_

#include "napi.h"

namespace felicia {

class ScopedEnvSetter {
 public:
  ScopedEnvSetter(Napi::Env env);
  ~ScopedEnvSetter();

  static napi_env CurrentEnv();
};

}  // namespace felicia

#endif  // FELICIA_JS_LIB_SCOPED_ENV_H_