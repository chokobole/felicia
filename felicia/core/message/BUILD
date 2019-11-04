load("//bazel:felicia.bzl", "if_has_ros")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "message",
    srcs = [
        "dynamic_protobuf_message.cc",
        "header.cc",
        "message_io_error.cc",
        "protobuf_loader.cc",
        "protobuf_message_io.cc",
        "protobuf_util.cc",
        "ros_header.cc",
        "ros_rpc_header.cc",
        "serialized_message.cc",
    ],
    hdrs = [
        "dynamic_protobuf_message.h",
        "header.h",
        "message_filter.h",
        "message_io.h",
        "message_io_error.h",
        "message_io_error_list.h",
        "protobuf_loader.h",
        "protobuf_message_io.h",
        "protobuf_util.h",
        "ros_header.h",
        "ros_header_io.h",
        "ros_message_io.h",
        "ros_protocol.h",
        "ros_rpc_header.h",
        "serialized_message.h",
        "serialized_message_io.h",
    ],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/util:command_line_interface",
    ] + if_has_ros([
        "@ros",
    ]),
)

fel_cc_library(
    name = "message_test_util",
    testonly = True,
    hdrs = [
        "test/a_message.h",
    ],
    deps = [
        ":message",
    ],
)

fel_cc_test(
    name = "message_tests",
    size = "small",
    srcs = [
        "message_filter_test.cc",
    ],
    deps = [
        ":message_test_util",
        "@com_google_googletest//:gtest_main",
    ],
)
