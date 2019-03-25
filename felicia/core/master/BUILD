load("//bazel:felicia.bzl", "fel_shared_library")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "client",
    srcs = ["client.cc"],
    hdrs = ["client.h"],
    deps = ["//felicia/core/node"],
)

fel_cc_library(
    name = "heart_beat_listener",
    srcs = ["heart_beat_listener.cc"],
    hdrs = ["heart_beat_listener.h"],
    deps = [
        "//felicia/core/channel",
        "//felicia/core/lib",
    ],
)

fel_cc_library(
    name = "heart_beat_signaller",
    srcs = ["heart_beat_signaller.cc"],
    hdrs = ["heart_beat_signaller.h"],
    deps = [":heart_beat_listener"],
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
    srcs = ["master_proxy.cc"],
    hdrs = ["master_proxy.h"],
    deps = [
        ":heart_beat_signaller",
        ":master_client_interface",
        ":topic_info_watcher",
        "//felicia/core/channel",
        "//felicia/core/master/rpc:grpc_master_client",
        "//felicia/core/master/rpc:grpc_util",
        "//felicia/core/master/rpc:master_service_proto_cc",
        "//felicia/core/node:node_lifecycle",
    ],
)

fel_cc_library(
    name = "master",
    srcs = [
        "master.cc",
    ],
    hdrs = [
        "errors.h",
        "master.h",
    ],
    deps = [
        ":client",
        ":heart_beat_listener",
    ],
)

fel_cc_library(
    name = "topic_info_watcher",
    srcs = ["topic_info_watcher.cc"],
    hdrs = ["topic_info_watcher.h"],
    deps = [
        "//felicia/core/channel",
        "//felicia/core/lib",
        "//felicia/core/protobuf:protos_all_cc",
    ],
)

fel_cc_test(
    name = "master_test",
    size = "small",
    srcs = ["master_test.cc"],
    deps = fel_shared_library() + [
        ":master",
        "//felicia/core/node",
        "@com_google_googletest//:gtest_main",
    ],
)
