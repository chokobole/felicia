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
    ],
    hdrs = [
        "channel.h",
        "channel_factory.h",
        "tcp_channel.h",
        "udp_channel.h",
        "shm_channel.h",
    ] + if_not_windows([
        "uds_channel.h",
    ]) + select({
        "//felicia:win_no_grpc": [],
        "//conditions:default": [
            "ws_channel.h",
        ],
    }),
    deps = [
        ":channel_impl",
        "//felicia/core/channel/shared_memory",
        "//felicia/core/channel/socket",
        "//felicia/core/lib",
        "//felicia/core/message",
    ],
)
