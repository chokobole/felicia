load("@env//:env.bzl", "FELICIA_ROOT")

def _felicia_env_impl(ctx):
    output = ctx.outputs.out
    ctx.actions.write(output = output, content =
                                           "#ifndef FELICIA_CORE_LIB_BASE_FELICIA_ENV_H_\n" +
                                           "#define FELICIA_CORE_LIB_BASE_FELICIA_ENV_H_\n" +
                                           "#define FELICIA_ROOT \"%s\"\n" % (FELICIA_ROOT) +
                                           "#endif  // FELICIA_CORE_LIB_BASE_FELICIA_ENV_H_")

felicia_env = rule(
    implementation = _felicia_env_impl,
    outputs = {"out": "%{name}.h"},
)
