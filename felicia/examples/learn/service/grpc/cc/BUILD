load("//bazel:felicia_cc.bzl", "fel_cc_binary")

package(default_visibility = ["//visibility:private"])

fel_cc_binary(
    name = "grpc_node_creator",
    srcs = [
        "grpc_client_node.cc",
        "grpc_client_node.h",
        "grpc_node_creator.cc",
        "grpc_server_node.cc",
        "grpc_server_node.h",
    ],
    deps = [
        "//felicia/core:felicia_init",
        "//felicia/core/communication",
        "//felicia/examples/learn/service/common/cc:simple_service_flag",
        "//felicia/examples/learn/service/grpc:simple_service_proto_cc",
    ],
)
