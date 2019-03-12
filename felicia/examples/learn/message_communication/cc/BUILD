package(default_visibility = ["//visibility:private"])

licenses(["notice"])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)

cc_library(
    name = "node_create_flag",
    srcs = ["node_create_flag.cc"],
    hdrs = ["node_create_flag.h"],
    copts = fel_copts(),
    visibility = ["//felicia/examples/learn/message_communication/protobuf:internal"],
    deps = [
        "//felicia/core/util:command_line_interface",
    ],
)

cc_library(
    name = "simple_publishing_node",
    srcs = ["simple_publishing_node.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/cc:communication",
        "//felicia/core:node",
        "//felicia/examples/learn/message_communication/protobuf:message_spec_proto_cc",
    ],
)

cc_library(
    name = "simple_subscribing_node",
    srcs = ["simple_subscribing_node.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/cc:communication",
        "//felicia/core:node",
        "//felicia/examples/learn/message_communication/protobuf:message_spec_proto_cc",
    ],
)

cc_binary(
    name = "node_creator",
    srcs = ["node_creator.cc"],
    copts = fel_copts(),
    deps = [
        ":node_create_flag",
        ":simple_publishing_node",
        ":simple_subscribing_node",
    ],
)
