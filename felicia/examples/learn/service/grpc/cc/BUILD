load("//bazel:felicia.bzl", "fel_shared_library")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
    "fel_cc_library",
)

package(default_visibility = ["//visibility:private"])

fel_cc_binary(
    name = "grpc_node_creator",
    srcs = [
        "grpc_node_creator.cc",
        "grpc_service_flag.cc",
        "grpc_service_flag.h",
    ],
    deps = fel_shared_library() + [
        "//felicia/core:felicia_init",
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
        "//felicia/examples/learn/service/common/cc:service_create_flag",
        "//felicia/examples/learn/service/grpc:simple_service_proto_cc",
    ],
)
