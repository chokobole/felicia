load("//bazel:felicia.bzl", "fel_shared_library")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "socket",
    srcs = [
        "permessage_deflate.cc",
        "socket.cc",
        "tcp_client_socket.cc",
        "tcp_server_socket.cc",
        "tcp_socket.cc",
        "udp_client_socket.cc",
        "udp_server_socket.cc",
        "udp_socket.cc",
        "web_socket.cc",
        "web_socket_extension.cc",
        "web_socket_server.cc",
    ],
    hdrs = [
        "permessage_deflate.h",
        "socket.h",
        "tcp_client_socket.h",
        "tcp_server_socket.h",
        "tcp_socket.h",
        "udp_client_socket.h",
        "udp_server_socket.h",
        "udp_socket.h",
        "web_socket.h",
        "web_socket_extension.h",
        "web_socket_server.h",
    ],
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
