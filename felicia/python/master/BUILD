load("//bazel:felicia.bzl", "if_not_windows")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "master_py_cc",
    srcs = [
        "master_proxy_py.cc",
    ],
    hdrs = [
        "master_proxy_py.h",
    ],
    copts = if_not_windows([
        "-fvisibility=hidden",
    ]),
    deps = [
        "//felicia/core/master:master_proxy",
        "//felicia/python/type_conversion",
        "@pybind11",
    ],
)
