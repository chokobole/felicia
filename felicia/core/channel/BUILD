load("//bazel:felicia_cc.bzl", "fel_cc_library")
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "channel_proto",
    srcs = ["channel.proto"],
    visibility = ["//felicia:internal"],
)

fel_cc_library(
    name = "channel",
    srcs = [
        "channel.cc",
        "channel_base.cc",
        "tcp_channel_base.cc",
        "tcp_client_channel.cc",
        "tcp_server_channel.cc",
        "udp_channel_base.cc",
        "udp_client_channel.cc",
        "udp_server_channel.cc",
    ],
    hdrs = [
        "channel.h",
        "channel_base.h",
        "channel_factory.h",
        "tcp_channel.h",
        "tcp_channel_base.h",
        "tcp_client_channel.h",
        "tcp_server_channel.h",
        "udp_channel.h",
        "udp_channel_base.h",
        "udp_client_channel.h",
        "udp_server_channel.h",
    ],
    deps = [
        ":channel_proto_cc",
        "//felicia/core/lib",
        "//felicia/core/message",
    ],
)
