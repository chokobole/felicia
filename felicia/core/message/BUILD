load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "message",
    srcs = [
        "dynamic_protobuf_message.cc",
        "header.cc",
        "message_io.cc",
        "protobuf_loader.cc",
        "protobuf_util.cc",
    ],
    hdrs = [
        "dynamic_protobuf_message.h",
        "header.h",
        "message_io.h",
        "message_io_error_list.h",
        "protobuf_loader.h",
        "protobuf_util.h",
    ],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/util:command_line_interface",
    ],
)
