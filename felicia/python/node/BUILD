load("//bazel:felicia.bzl", "if_not_windows")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "node_lifecycle_py_cc",
    srcs = [
        "node_lifecycle_py.cc",
    ],
    hdrs = [
        "node_lifecycle_py.h",
    ],
    copts = if_not_windows([
        "-fvisibility=hidden",
    ]),
    deps = [
        "//felicia/core/node:node_lifecycle",
        "//felicia/python/type_conversion",
        "@pybind11",
    ],
)
