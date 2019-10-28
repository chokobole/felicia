load("//bazel:felicia.bzl", "if_not_windows")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "message",
    srcs = [
        "message_util.cc",
    ],
    hdrs = [
        "message_util.h",
    ],
    copts = if_not_windows([
        "-fvisibility=hidden",
    ]),
    deps = [
        "//felicia/core/message",
        "@pybind11",
    ],
)
