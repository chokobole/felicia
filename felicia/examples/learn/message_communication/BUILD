package(default_visibility = ["//visibility:private"])
licenses(["notice"])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)

load(
    "//felicia/core:platform/build_config.bzl",
    "fel_proto_library_cc",
)

fel_proto_library_cc(
    name = "message_spec_proto",
    srcs = ["protobuf/message_spec.proto"]
)

cc_library(
    name = "node_create_flag",
    srcs = ["node_create_flag.cc"],
    hdrs = ["node_create_flag.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core/util:command_line_interface",
    ]
)

cc_library(
    name = "simple_publishing_node",
    srcs = ["simple_publishing_node.h"],
    copts = fel_copts(),
    deps = [
        ":message_spec_proto_cc",
        "//felicia/cc:communication",
        "//felicia/core:node",
    ]
)

cc_library(
    name = "simple_subscribing_node",
    srcs = ["simple_subscribing_node.h"],
    copts = fel_copts(),
    deps = [
        ":message_spec_proto_cc",
        "//felicia/cc:communication",
        "//felicia/core:node",
    ]
)

cc_binary(
    name = "node_creator",
    srcs = ["node_creator.cc"],
    copts = fel_copts(),
    deps = [
        ":message_spec_proto_cc",
        ":node_create_flag",
        ":simple_publishing_node",
        ":simple_subscribing_node",
    ]
)