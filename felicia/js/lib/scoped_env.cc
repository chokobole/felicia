#include "felicia/js/lib/scoped_env.h"

#if defined(FEL_NODE_BINDING)

namespace felicia {

namespace {

napi_env g_current_env;

}  // namespace

ScopedEnvSetter::ScopedEnvSetter(Napi::Env env) {
  g_current_env = napi_env(env);
}

ScopedEnvSetter::~ScopedEnvSetter() { g_current_env = nullptr; }

// static
napi_env ScopedEnvSetter::CurrentEnv() { return g_current_env; }

}  // namespace felicia

#endif  // defined(FEL_NODE_BINDING)