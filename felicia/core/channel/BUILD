# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia.bzl", "if_not_windows")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "channel_impl",
    srcs = ["channel_impl.cc"],
    hdrs = [
        "channel_impl.h",
        "settings.h",
    ],
    deps = ["//felicia/core/lib"],
)

fel_cc_library(
    name = "channel",
    srcs = [
        "channel.cc",
        "channel_buffer.cc",
        "channel_factory.cc",
        "ros_service_request.cc",
        "ros_service_response.cc",
        "ros_topic_request.cc",
        "ros_topic_response.cc",
        "tcp_channel.cc",
        "udp_channel.cc",
        "shm_channel.cc",
        "ws_channel.cc",
    ] + if_not_windows([
        "uds_channel.cc",
    ]),
    hdrs = [
        "channel.h",
        "channel_factory.h",
        "channel_buffer.h",
        "message_sender.h",
        "message_receiver.h",
        "ros_service_request.h",
        "ros_service_response.h",
        "ros_topic_request.h",
        "ros_topic_response.h",
        "tcp_channel.h",
        "udp_channel.h",
        "shm_channel.h",
        "ws_channel.h",
    ] + if_not_windows([
        "uds_channel.h",
    ]),
    deps = [
        ":channel_impl",
        "//felicia/core/channel/shared_memory",
        "//felicia/core/channel/socket",
        "//felicia/core/lib",
        "//felicia/core/message",
    ],
)
