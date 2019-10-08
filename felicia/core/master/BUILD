load(
    "//bazel:felicia.bzl",
    "fel_shared_library",
    "if_has_ros",
    "if_not_windows",
)
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "bytes_constants",
    srcs = ["bytes_constants.cc"],
    deps = ["//felicia/core/lib"],
)

fel_cc_library(
    name = "heart_beat",
    srcs = [
        "heart_beat_listener.cc",
        "heart_beat_signaller.cc",
    ],
    hdrs = [
        "heart_beat_listener.h",
        "heart_beat_signaller.h",
    ],
    deps = [
        ":bytes_constants",
        "//felicia/core/channel",
        "//felicia/core/lib",
    ],
)

fel_cc_library(
    name = "master_client_interface",
    srcs = ["master_client_interface.cc"],
    hdrs = ["master_client_interface.h"],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/protobuf:protos_all_cc",
    ],
)

fel_cc_library(
    name = "master_proxy",
    srcs = [
        "master_proxy.cc",
        "topic_info_watcher.cc",
        "topic_info_watcher.h",
    ],
    hdrs = [
        "master_proxy.h",
    ],
    deps = [
        ":heart_beat",
        ":master_client_interface",
        "//felicia/core/channel",
        "//felicia/core/node:node_lifecycle",
    ] + select({
        "//felicia:win_no_grpc": [],
        "//conditions:default": [
            "//felicia/core/master/rpc:grpc_master_client",
            "//felicia/core/master/rpc:grpc_util",
        ],
    }),
)

fel_cc_library(
    name = "master",
    srcs = [
        "client.cc",
        "client.h",
        "master.cc",
    ] + if_has_ros([
        "ros_master_proxy.cc",
    ]),
    hdrs = [
        "errors.h",
        "master.h",
    ] + if_has_ros([
        "ros_master_proxy.h",
    ]),
    defines = if_has_ros([
        "HAS_ROS",
    ]),
    deps = [
        ":heart_beat",
        "//felicia/core/node",
        "//felicia/core/util",
    ] + if_has_ros([
        "@ros",
    ]),
)

fel_cc_test(
    name = "master_test",
    size = "small",
    srcs = if_not_windows(["master_test.cc"]),
    deps = fel_shared_library() + [
        ":master",
        "//felicia/core/node",
        "@com_google_googletest//:gtest_main",
    ],
)
