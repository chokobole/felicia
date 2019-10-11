load("//bazel:felicia_cc.bzl", "fel_cc_library")

fel_cc_library(
    name = "topic_create_flag",
    srcs = ["topic_create_flag.cc"],
    hdrs = ["topic_create_flag.h"],
    visibility = ["//felicia/examples:internal"],
    deps = ["//felicia/examples/learn/common/cc:node_create_flag"],
)

fel_cc_library(
    name = "camera_flag",
    srcs = ["camera_flag.cc"],
    hdrs = ["camera_flag.h"],
    visibility = ["//felicia/examples:internal"],
    deps = [":topic_create_flag"],
)
