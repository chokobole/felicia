# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia.bzl", "if_not_windows")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "socket",
    srcs = [
        "host_resolver.cc",
        "permessage_deflate.cc",
        "socket.cc",
        "socket_bio_adapter.cc",
        "stream_socket_broadcaster.cc",
        "ssl_client_socket.cc",
        "ssl_server_context.cc",
        "ssl_server_socket.cc",
        "ssl_socket.cc",
        "tcp_client_socket.cc",
        "tcp_server_socket.cc",
        "tcp_socket.cc",
        "udp_client_socket.cc",
        "udp_server_socket.cc",
        "udp_socket.cc",
        "web_socket.cc",
        "web_socket_basic_stream.cc",
        "web_socket_channel.cc",
        "web_socket_channel_broadcaster.cc",
        "web_socket_deflate_stream.cc",
        "web_socket_extension.cc",
        "web_socket_server.cc",
    ] + if_not_windows([
        "uds_endpoint.cc",
        "unix_domain_client_socket.cc",
        "unix_domain_server_socket.cc",
        "unix_domain_socket.cc",
    ]),
    hdrs = [
        "datagram_socket.h",
        "host_resolver.h",
        "permessage_deflate.h",
        "socket.h",
        "socket_bio_adapter.h",
        "stream_socket_broadcaster.h",
        "ssl_client_socket.h",
        "ssl_server_context.h",
        "ssl_server_socket.h",
        "ssl_socket.h",
        "stream_socket.h",
        "tcp_client_socket.h",
        "tcp_server_socket.h",
        "tcp_socket.h",
        "udp_client_socket.h",
        "udp_server_socket.h",
        "udp_socket.h",
        "web_socket.h",
        "web_socket_basic_stream.h",
        "web_socket_channel.h",
        "web_socket_channel_broadcaster.h",
        "web_socket_deflate_stream.h",
        "web_socket_extension.h",
        "web_socket_server.h",
        "web_socket_stream.h",
    ] + if_not_windows([
        "uds_endpoint.h",
        "unix_domain_client_socket.h",
        "unix_domain_server_socket.h",
        "unix_domain_socket.h",
    ]),
    deps = [
        "//felicia/core/channel:channel_impl",
    ],
)

fel_cc_test(
    name = "web_socket_tests",
    size = "small",
    srcs = [
        "permessage_deflate_test.cc",
        "web_socket_test.cc",
    ],
    deps = [
        ":socket",
        "@com_google_googletest//:gtest_main",
    ],
)
