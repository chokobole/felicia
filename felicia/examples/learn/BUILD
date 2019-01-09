package(default_visibility = ["//visibility:public"])

licenses(["notice"]) # Apache 2.0

exports_files(["LICENSE"])

PROTO_SRCS = [
    "protobuf/message_spec.proto",
]

proto_library(
    name = "proto",
    srcs = PROTO_SRCS,
)

cc_proto_library(
    name = "proto_cc",
    deps = [":proto"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "simple_message_publisher",
    srcs = ["simple_message_publisher.cc"],
    deps=[
        "//felicia/core:lib",
        "//felicia/core:core",
        ":proto_cc",
    ],
)

cc_binary(
    name = "simple_message_subscriber",
    srcs = ["simple_message_subscriber.cc"],
    deps=[
        "//felicia/core:lib",
        "//felicia/core:core",
        ":proto_cc",
    ],
)