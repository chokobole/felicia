load("//bazel:felicia_cc.bzl", "fel_cc_library")

fel_cc_library(
    name = "service_create_flag",
    srcs = ["service_create_flag.cc"],
    hdrs = ["service_create_flag.h"],
    is_external = False,
    visibility = ["//felicia/examples:internal"],
    deps = ["//felicia/examples/learn/common/cc:node_create_flag"],
)

fel_cc_library(
    name = "simple_service_flag",
    srcs = ["simple_service_flag.cc"],
    hdrs = ["simple_service_flag.h"],
    is_external = False,
    visibility = ["//felicia/examples:internal"],
    deps = [":service_create_flag"],
)
