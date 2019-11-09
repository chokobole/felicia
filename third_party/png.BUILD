# This is taken and modified from https://github.com/tensorflow/tensorflow/blob/127aae00a5d2c94045af5da772fed3173db42e69/third_party/png.BUILD

# Description:
#   libpng is the official PNG reference library.

licenses(["notice"])  # BSD/MIT-like license

exports_files(["LICENSE"])

cc_library(
    name = "png",
    srcs = [
        "png.c",
        "pngdebug.h",
        "pngerror.c",
        "pngget.c",
        "pnginfo.h",
        "pnglibconf.h",
        "pngmem.c",
        "pngpread.c",
        "pngpriv.h",
        "pngread.c",
        "pngrio.c",
        "pngrtran.c",
        "pngrutil.c",
        "pngset.c",
        "pngstruct.h",
        "pngtrans.c",
        "pngwio.c",
        "pngwrite.c",
        "pngwtran.c",
        "pngwutil.c",
    ] + select({
        ":windows": [
            "intel/intel_init.c",
            "intel/filter_sse2_intrinsics.c",
        ],
        "@com_github_chokobole_felicia//felicia:ppc": [
            "powerpc/powerpc_init.c",
            "powerpc/filter_vsx_intrinsics.c",
        ],
        "//conditions:default": [
        ],
    }),
    hdrs = [
        "png.h",
        "pngconf.h",
    ],
    copts = select({
        ":windows": ["-DPNG_INTEL_SSE_OPT=1"],
        "//conditions:default": [],
    }),
    includes = ["."],
    linkopts = select({
        ":windows": [],
        "//conditions:default": ["-lm"],
    }),
    visibility = ["//visibility:public"],
    # FIXME: Can use @felicia/bazel:fel_zlib_deps()?
    deps = select({
        "@com_github_chokobole_felicia//felicia:win_no_grpc": ["@node_addon_api"],
        "//conditions:default": ["//external:zlib"],
    }),
)

genrule(
    name = "snappy_stubs_public_h",
    srcs = ["scripts/pnglibconf.h.prebuilt"],
    outs = ["pnglibconf.h"],
    cmd = "sed -e 's/PNG_ZLIB_VERNUM 0/PNG_ZLIB_VERNUM 0x12b0/' $< >$@",
)

config_setting(
    name = "windows",
    values = {"cpu": "x64_windows"},
)
