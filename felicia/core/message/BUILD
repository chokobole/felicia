# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load(
    "//bazel:felicia_cc.bzl",
    "define",
    "fel_cc_library",
    "fel_cc_test",
    "fel_cxxopts",
)
load("//bazel:felicia_proto.bzl", "fel_proto_library_cc")

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
    copts = fel_cxxopts(True) + define(["BAZEL_BUILD"]),
    deps = [
        "//felicia/core/lib",
        "//felicia/core/util:command_line_interface",
    ],
)

fel_proto_library_cc(
    name = "simple_message_proto",
    testonly = True,
    srcs = [
        "test/simple_message.proto",
    ],
    visibility = ["//visibility:private"],
)

fel_proto_library_cc(
    name = "simple_service_proto",
    testonly = True,
    srcs = [
        "test/simple_service.proto",
    ],
    has_services = True,
    cc_grpc_version = True,
    visibility = ["//visibility:private"],
)

fel_cc_library(
    name = "message_test_util",
    testonly = True,
    hdrs = [
        "test/simple_message.h",
    ],
    deps = [
        ":message",
        ":simple_message_proto_cc",
        ":simple_service_proto_cc",
    ],
)

fel_cc_test(
    name = "message_unittests",
    size = "small",
    srcs = [
        "message_filter_unittest.cc",
    ],
    deps = [
        ":message_test_util",
        "@com_google_googletest//:gtest_main",
    ],
)
