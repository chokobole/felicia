load("//bazel:felicia.bzl", "fel_shared_library")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
    "fel_cc_library",
)

package(default_visibility = ["//visibility:private"])

fel_cc_library(
    name = "node_create_flag",
    srcs = ["node_create_flag.cc"],
    hdrs = ["node_create_flag.h"],
    deps = ["//felicia/core/util:command_line_interface"],
)

fel_cc_library(
    name = "simple_publishing_node",
    srcs = ["simple_publishing_node.h"],
    deps = [
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
        "//felicia/examples/learn/message_communication/protobuf:message_spec_proto_cc",
    ],
)

fel_cc_library(
    name = "simple_subscribing_node",
    srcs = ["simple_subscribing_node.h"],
    deps = [
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
        "//felicia/examples/learn/message_communication/protobuf:message_spec_proto_cc",
    ],
)

fel_cc_binary(
    name = "node_creator",
    srcs = ["node_creator.cc"],
    deps = fel_shared_library() + [
        ":node_create_flag",
        ":simple_publishing_node",
        ":simple_subscribing_node",
        "//felicia/core:felicia_init",
    ],
)
