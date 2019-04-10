load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "message",
    srcs = [
        "dynamic_protobuf_message.cc",
        "header.cc",
        "protobuf_loader.cc",
    ],
    hdrs = [
        "dynamic_protobuf_message.h",
        "header.h",
        "message_io.h",
        "protobuf_loader.h",
    ],
    deps = [
        "//felicia/core/lib",
    ],
)
