load("//bazel:felicia.bzl", "if_not_windows")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "channel_py_cc",
    srcs = [
        "channel_py.cc",
    ],
    hdrs = [
        "channel_py.h",
    ],
    copts = if_not_windows([
        "-fvisibility=hidden",
    ]),
    deps = [
        "//felicia/core/channel",
        "//felicia/python/type_conversion",
        "@pybind11",
    ],
)
