package(default_visibility = ["//visibility:public"])
licenses(["notice"]) # Apache 2.0

exports_files(["LICENSE"])

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
    srcs = ["message_communication/protobuf/message_spec.proto"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "node_create_flag_parser_delegate",
    srcs = ["message_communication/node_create_flag_parser_delegate.cc"],
    hdrs = ["message_communication/node_create_flag_parser_delegate.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core/util:command_line_interface",
    ]
)

cc_library(
    name = "simple_publishing_node",
    srcs = ["message_communication/simple_publishing_node.h"],
    copts = fel_copts(),
    deps = [
        ":message_spec_proto_cc",
        "//felicia/cc:communication",
        "//felicia/core:node",
    ]
)

cc_library(
    name = "simple_subscribing_node",
    srcs = ["message_communication/simple_subscribing_node.h"],
    copts = fel_copts(),
    deps = [
        ":message_spec_proto_cc",
        "//felicia/cc:communication",
        "//felicia/core:node",
    ]
)

cc_binary(
    name = "node_creator",
    srcs = ["message_communication/node_creator.cc"],
    copts = fel_copts(),
    deps = [
        ":message_spec_proto_cc",
        ":node_create_flag_parser_delegate",
        ":simple_publishing_node",
        ":simple_subscribing_node",
        "//felicia/cc:master_proxy",
    ]
)