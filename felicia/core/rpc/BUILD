# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "rpc",
    srcs = [
        "client_interface.cc",
        "grpc_util.cc",
        "ros_serialized_service_interface.cc",
        "server_interface.cc",
    ],
    hdrs = [
        "client.h",
        "client_interface.h",
        "grpc_async_client_call.h",
        "grpc_call.h",
        "grpc_client_cq_tag.h",
        "grpc_client_impl.h",
        "grpc_server_impl.h",
        "grpc_service_impl.h",
        "grpc_util.h",
        "ros_client_impl.h",
        "ros_serialized_service.h",
        "ros_serialized_service_interface.h",
        "ros_server_impl.h",
        "ros_service_handler.h",
        "ros_service_impl.h",
        "ros_service_manager.h",
        "ros_util.h",
        "server.h",
        "server_interface.h",
        "service.h",
    ],
    deps = [
        "//felicia:grpc++",
        "//felicia/core/channel",
        "//felicia/core/lib",
    ],
)
