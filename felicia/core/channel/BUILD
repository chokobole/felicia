load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "channel",
    srcs = [
        "channel.cc",
        "channel_impl.cc",
        "socket/socket.cc",
        "socket/tcp_client_socket.cc",
        "socket/tcp_server_socket.cc",
        "socket/tcp_socket.cc",
        "socket/udp_client_socket.cc",
        "socket/udp_server_socket.cc",
        "socket/udp_socket.cc",
        "socket/web_socket.cc",
        "socket/web_socket_server.cc",
    ],
    hdrs = [
        "channel.h",
        "channel_factory.h",
        "channel_impl.h",
        "socket/socket.h",
        "socket/tcp_client_socket.h",
        "socket/tcp_server_socket.h",
        "socket/tcp_socket.h",
        "socket/udp_client_socket.h",
        "socket/udp_server_socket.h",
        "socket/udp_socket.h",
        "socket/web_socket.h",
        "socket/web_socket_server.h",
        "tcp_channel.h",
        "udp_channel.h",
        "ws_channel.h",
    ],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/message",
        "//felicia/core/protobuf:protos_all_cc",
    ],
)
