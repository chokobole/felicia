# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "lidar_proto",
    srcs = [
        "lidar_frame_message.proto",
    ],
    default_header = True,
    export_proto = True,
    visibility = ["//visibility:public"],
)

fel_cc_library(
    name = "lidar",
    srcs = [
        "lidar_endpoint.cc",
        "lidar_frame.cc",
        "lidar_interface.cc",
    ],
    hdrs = [
        "lidar_endpoint.h",
        "lidar_frame.h",
        "lidar_interface.h",
        "lidar_state.h",
    ],
    deps = [
        "//felicia/core/lib",
    ],
)
