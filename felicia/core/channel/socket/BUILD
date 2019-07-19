load(
    "//bazel:felicia.bzl",
    "fel_shared_library",
    "if_not_windows",
)
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "socket",
    srcs = [
        "socket.cc",
        "stream_socket_broadcaster.cc",
        "tcp_client_socket.cc",
        "tcp_server_socket.cc",
        "tcp_socket.cc",
        "udp_client_socket.cc",
        "udp_server_socket.cc",
        "udp_socket.cc",
    ] + if_not_windows([
        "uds_endpoint.cc",
        "unix_domain_client_socket.cc",
        "unix_domain_server_socket.cc",
        "unix_domain_socket.cc",
    ]) + select({
        "//felicia:win_no_grpc": [],
        "//conditions:default": [
            "permessage_deflate.cc",
            "web_socket.cc",
            "web_socket_basic_stream.cc",
            "web_socket_channel.cc",
            "web_socket_channel_broadcaster.cc",
            "web_socket_deflate_stream.cc",
            "web_socket_extension.cc",
            "web_socket_server.cc",
        ],
    }),
    hdrs = [
        "datagram_socket.h",
        "socket.h",
        "stream_socket_broadcaster.h",
        "stream_socket.h",
        "tcp_client_socket.h",
        "tcp_server_socket.h",
        "tcp_socket.h",
        "udp_client_socket.h",
        "udp_server_socket.h",
        "udp_socket.h",
    ] + if_not_windows([
        "uds_endpoint.h",
        "unix_domain_client_socket.h",
        "unix_domain_server_socket.h",
        "unix_domain_socket.h",
    ]) + select({
        "//felicia:win_no_grpc": [],
        "//conditions:default": [
            "permessage_deflate.h",
            "web_socket.h",
            "web_socket_basic_stream.h",
            "web_socket_channel.h",
            "web_socket_channel_broadcaster.h",
            "web_socket_deflate_stream.h",
            "web_socket_extension.h",
            "web_socket_server.h",
            "web_socket_stream.h",
        ],
    }),
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
    deps = fel_shared_library() + [
        ":socket",
        "@com_google_googletest//:gtest_main",
    ],
)
