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

cc_binary(
    name = "simple_message_publisher",
    srcs = ["message_communication/simple_message_publisher.cc"],
    copts = fel_copts(),
    deps=[
        ":message_spec_proto_cc",
        ":node_create_flag_parser_delegate",
        "//felicia/cc:communication",
        "//felicia/cc:master_proxy",
    ]
)

cc_binary(
    name = "simple_message_subscriber",
    srcs = ["message_communication/simple_message_subscriber.cc"],
    copts = fel_copts(),
    deps=[
        ":message_spec_proto_cc",
        ":node_create_flag_parser_delegate",
        "//felicia/cc:communication",
        "//felicia/cc:master_proxy",
    ]
)