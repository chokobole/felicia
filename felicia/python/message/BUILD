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
    deps = [
        "//felicia/core/message",
        "@pybind11",
    ],
)
