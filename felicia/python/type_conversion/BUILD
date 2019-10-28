load("//bazel:felicia.bzl", "if_not_windows")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "type_conversion",
    srcs = [
        "util.cc",
    ],
    hdrs = [
        "callback.h",
        "callback_forward.h",
        "callback_holder.h",
        "protobuf.h",
        "util.h",
    ],
    copts = if_not_windows([
        "-fvisibility=hidden",
    ]),
    deps = [
        "//felicia/core/lib",
        "//felicia/python/message",
        "@pybind11",
    ],
)
