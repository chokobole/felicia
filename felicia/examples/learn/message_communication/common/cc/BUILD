load("//bazel:felicia_cc.bzl", "fel_cc_library")

fel_cc_library(
    name = "node_create_flag",
    srcs = ["node_create_flag.cc"],
    hdrs = ["node_create_flag.h"],
    visibility = ["//felicia/examples/learn/message_communication:internal"],
    deps = ["//felicia/core/util:command_line_interface"],
)

fel_cc_library(
    name = "camera_flag",
    srcs = ["camera_flag.cc"],
    hdrs = ["camera_flag.h"],
    visibility = ["//felicia/examples/learn/message_communication:internal"],
    deps = [":node_create_flag"],
)
