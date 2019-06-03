load("//bazel:felicia.bzl", "tpl")
load("@env//:env.bzl", "FELICIA_ROOT")
load("@local_config_python//:py.bzl", "PYTHON2_BIN")
load(":cc_toolchain_config.bzl", "cc_toolchain_config")

package(default_visibility = ["//visibility:public"])

cc_toolchain_suite(
    name = "emscripten",
    toolchains = {
        "wasm": ":wasm_toolchain",
    },
)

filegroup(
    name = "all",
    srcs = [
        "emar.sh",
        "emcc.sh",
        ":emscripten_cache_content",
        ":env_sh",
        "@emscripten_clang//:all",
        "@emscripten_toolchain//:all",
    ],
)

filegroup(
    name = "emscripten_cache_content",
    srcs = glob(["emscripten_cache/**/*"]),
)

tpl(
    name = "env_sh",
    output = "env.sh",
    substitutions = {
        "%{BAZEL_FELICIA}": "%s/bazel-felicia" % FELICIA_ROOT,
        "%{PYTHON2_BIN}": PYTHON2_BIN,
    },
    template = "env.sh.tpl",
)

cc_toolchain(
    name = "wasm_toolchain",
    all_files = ":all",
    compiler_files = ":all",
    cpu = "wasm",
    dwp_files = ":empty",
    linker_files = ":all",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
    toolchain_config = ":wasm_toolchain_config",
    toolchain_identifier = "wasm-toolchain",
)

cc_toolchain_config(name = "wasm_toolchain_config")
