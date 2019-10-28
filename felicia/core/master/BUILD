load("//bazel:felicia.bzl", "if_not_windows")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "bytes_constants",
    srcs = ["bytes_constants.cc"],
    hdrs = ["bytes_constants.h"],
    deps = ["//felicia/core/lib"],
)

fel_cc_library(
    name = "master_client_interface",
    srcs = ["master_client_interface.cc"],
    hdrs = ["master_client_interface.h"],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/master/rpc:master_method_list",
    ],
)

fel_cc_library(
    name = "heart_beat_signaller",
    srcs = ["heart_beat_signaller.cc"],
    hdrs = ["heart_beat_signaller.h"],
    deps = [
        ":bytes_constants",
        "//felicia/core/channel",
    ],
)

fel_cc_library(
    name = "master_proxy",
    srcs = [
        "master_notification_watcher.cc",
        "master_proxy.cc",
    ],
    hdrs = [
        "master_notification_watcher.h",
        "master_proxy.h",
    ],
    deps = [
        ":bytes_constants",
        ":heart_beat_signaller",
        ":master_client_interface",
        "//felicia/core/channel",
        "//felicia/core/node:node_lifecycle",
    ] + select({
        "//felicia:win_no_grpc": [],
        "//conditions:default": [
            "//felicia/core/master/rpc:master_client",
            "//felicia/core/master/rpc:master_server_info",
        ],
    }),
)

fel_cc_library(
    name = "master",
    srcs = [
        "client.cc",
        "client.h",
        "errors.h",
        "heart_beat_listener.cc",
        "heart_beat_listener.h",
        "master.cc",
        "master.h",
        "node.cc",
        "node.h",
        "ros_master_proxy.cc",
        "ros_master_proxy.h",
    ],
    is_external = False,
    deps = [
        ":bytes_constants",
        ":heart_beat_signaller",
        "//felicia/core/channel",
        "//felicia/core/master/rpc:master_method_list",
        "//felicia/core/util",
    ],
)

fel_cc_test(
    name = "master_test",
    size = "small",
    srcs = if_not_windows(["master_test.cc"]),
    deps = [
        ":master",
        "@com_google_googletest//:gtest_main",
    ],
)
